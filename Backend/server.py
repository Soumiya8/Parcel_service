import json
import os
import re
import subprocess
from http import HTTPStatus
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import unquote, urlparse

try:
    from flask import Flask, jsonify, request, send_from_directory
except ModuleNotFoundError:
    Flask = None


BASE_DIR = Path(__file__).resolve().parent
FRONTEND_DIR = BASE_DIR.parent / "Frontend"
EXE_PATH = BASE_DIR / "app.exe"
DATA_PATH = BASE_DIR / "data.txt"
PARCELS_PATH = BASE_DIR / "parcels.txt"
USERS_PATH = BASE_DIR / "users.txt"
HISTORY_PATH = BASE_DIR / "delivery_history.txt"
NOTIFICATIONS_PATH = BASE_DIR / "notifications.txt"

PHONE_RE = re.compile(r"^\+91\d{10}$")


def ensure_storage_files():
    for path in (PARCELS_PATH, USERS_PATH, HISTORY_PATH, NOTIFICATIONS_PATH):
        path.touch(exist_ok=True)

    if PARCELS_PATH.stat().st_size == 0 and DATA_PATH.exists() and DATA_PATH.stat().st_size > 0:
        PARCELS_PATH.write_text(DATA_PATH.read_text(encoding="utf-8"), encoding="utf-8")

    if USERS_PATH.stat().st_size == 0:
        USERS_PATH.write_text("admin|admin123|admin\nuser|user123|user\n", encoding="utf-8")


ensure_storage_files()


def clean(value):
    return str(value or "").strip()


def run_c(*args):
    ensure_storage_files()

    if not EXE_PATH.exists():
        return {"success": False, "error": "app.exe not found. Compile the C backend first."}, 500

    try:
        result = subprocess.run(
            [str(EXE_PATH), *[str(arg) for arg in args]],
            cwd=BASE_DIR,
            text=True,
            capture_output=True,
            timeout=10,
        )
    except subprocess.TimeoutExpired:
        return {"success": False, "error": "C backend timed out"}, 500

    output = (result.stdout or "").strip()
    if not output:
        return {
            "success": False,
            "error": "C backend returned empty output",
            "details": (result.stderr or "").strip(),
        }, 500

    try:
        payload = json.loads(output)
    except json.JSONDecodeError:
        return {"success": False, "error": "Invalid JSON from C backend", "raw": output}, 500

    status = 200 if payload.get("success") else 400
    if result.returncode != 0 and status == 200:
        status = 500
    return payload, status


def validate_required(data, fields):
    missing = [field for field in fields if not clean(data.get(field))]
    if missing:
        return {"success": False, "error": "Missing required fields", "fields": missing}, 400
    return None


def login_from_file(data):
    username = clean(data.get("username"))
    password = clean(data.get("password"))
    role = clean(data.get("role"))

    if not username or not password or role not in {"admin", "user"}:
        return {"success": False, "message": "Invalid username or password"}, 401

    ensure_storage_files()
    for line in USERS_PATH.read_text(encoding="utf-8").splitlines():
        parts = line.strip().split("|")
        if len(parts) != 3:
            continue
        saved_username, saved_password, saved_role = parts
        if username == saved_username and password == saved_password and role == saved_role:
            return {"success": True, "role": saved_role}, 200

    return {"success": False, "message": "Invalid username or password"}, 401


def handle_api(method, path, data=None):
    data = data or {}

    if method == "GET" and path == "/api/stats":
        return run_c("stats")

    if method == "GET" and path == "/api/list":
        return run_c("list", "all", "all")

    if method == "POST" and path == "/api/login":
        return login_from_file(data)

    if method == "GET" and path.startswith("/api/track/"):
        return run_c("track", path.removeprefix("/api/track/"))

    if method == "POST" and path == "/api/add":
        expected_fields = [
            "senderName",
            "senderAddress",
            "senderContact",
            "receiverName",
            "receiverAddress",
            "receiverContact",
            "destinationOffice",
            "deliveryDate",
            "roadRoute",
            "priority",
        ]
        parcel = {
            "senderName": clean(data.get("senderName")),
            "senderAddress": clean(data.get("senderAddress")),
            "senderContact": clean(data.get("senderContact")),
            "receiverName": clean(data.get("receiverName")),
            "receiverAddress": clean(data.get("receiverAddress")),
            "receiverContact": clean(data.get("receiverContact")),
            "destinationOffice": clean(data.get("destinationOffice")),
            "deliveryDate": clean(data.get("deliveryDate")),
            "roadRoute": clean(data.get("roadRoute")),
        }
        missing = [key for key in expected_fields if not clean(data.get(key))]
        if missing:
            return {
                "success": False,
                "error": "Missing required fields for parcel creation",
                "fields": missing,
                "expectedKeys": expected_fields,
            }, 400

        priority = clean(data.get("priority"))
        if priority not in {"1", "2"}:
            return {"success": False, "error": "Priority must be 1 or 2", "fields": ["priority"]}, 400

        if not PHONE_RE.fullmatch(parcel["senderContact"]) or not PHONE_RE.fullmatch(parcel["receiverContact"]):
            return {"success": False, "error": "Phone number must be +91 followed by 10 digits"}, 400

        payload, status = run_c(
            "add",
            parcel["senderName"],
            parcel["senderAddress"],
            parcel["senderContact"],
            parcel["receiverName"],
            parcel["receiverAddress"],
            parcel["receiverContact"],
            parcel["destinationOffice"],
            parcel["deliveryDate"],
            parcel["roadRoute"],
            priority,
        )
        if status != 200 or not payload.get("success"):
            return payload, status

        created = payload.get("parcel", {})
        tracking_id = created.get("trackingID")
        return {
            "success": True,
            "trackingID": tracking_id,
            "parcel": created,
            "message": "Parcel created successfully",
        }, 200

    if method == "POST" and path in {"/api/update", "/api/update-status"}:
        error = validate_required(data, ["trackingID", "status"])
        if error:
            return error
        return run_c("update", clean(data.get("trackingID")), clean(data.get("status")))

    if method == "POST" and path.startswith("/api/failed/"):
        return run_c("failed", path.removeprefix("/api/failed/"))

    if method == "GET" and path.startswith("/api/filter/location/"):
        return run_c("filter-location", path.removeprefix("/api/filter/location/"))

    if method == "GET" and path.startswith("/api/filter/date/"):
        return run_c("filter-date", path.removeprefix("/api/filter/date/"))

    if method == "GET" and path.startswith("/api/routes/"):
        return run_c("routes", path.removeprefix("/api/routes/"))

    return {"success": False, "error": "API endpoint not found"}, 404


def create_flask_app():
    app = Flask(__name__, static_folder=None)

    @app.get("/")
    @app.get("/n.html")
    def index():
        return send_from_directory(FRONTEND_DIR, "n.html")

    @app.get("/style.css")
    def css():
        return send_from_directory(FRONTEND_DIR, "style.css")

    @app.get("/script.js")
    def js():
        return send_from_directory(FRONTEND_DIR, "script.js")

    @app.get("/Frontend/<path:filename>")
    def frontend_alias(filename):
        return send_from_directory(FRONTEND_DIR, filename)

    @app.route("/api/add", methods=["POST"])
    def api_add():
        data = request.get_json(silent=True) or {}
        payload, status = handle_api("POST", "/api/add", data)
        return jsonify(payload), status

    @app.route("/api/<path:api_path>", methods=["GET", "POST"])
    def api_router(api_path):
        data = request.get_json(silent=True) or {}
        payload, status = handle_api(request.method, f"/api/{api_path}", data)
        return jsonify(payload), status

    @app.errorhandler(404)
    def not_found(error):
        if request.path.startswith("/api/"):
            return jsonify({"success": False, "error": "API endpoint not found"}), 404
        return send_from_directory(FRONTEND_DIR, "n.html")

    return app


class FallbackHandler(BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        return

    def send_json(self, payload, status=200):
        body = json.dumps(payload).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def send_file(self, filename):
        file_path = (FRONTEND_DIR / filename).resolve()
        if not str(file_path).startswith(str(FRONTEND_DIR.resolve())) or not file_path.is_file():
            file_path = FRONTEND_DIR / "n.html"

        content_types = {
            ".html": "text/html",
            ".css": "text/css",
            ".js": "application/javascript",
        }
        content = file_path.read_bytes()
        self.send_response(HTTPStatus.OK)
        self.send_header("Content-Type", content_types.get(file_path.suffix, "application/octet-stream"))
        self.send_header("Content-Length", str(len(content)))
        self.end_headers()
        self.wfile.write(content)

    def do_GET(self):
        parsed = urlparse(self.path)
        path = unquote(parsed.path)

        if path.startswith("/api/"):
            payload, status = handle_api("GET", path)
            self.send_json(payload, status)
            return

        if path in {"/", "/n.html"}:
            self.send_file("n.html")
        elif path == "/style.css":
            self.send_file("style.css")
        elif path == "/script.js":
            self.send_file("script.js")
        elif path.startswith("/Frontend/"):
            self.send_file(path.removeprefix("/Frontend/"))
        else:
            self.send_file("n.html")

    def do_POST(self):
        parsed = urlparse(self.path)
        path = unquote(parsed.path)
        length = int(self.headers.get("Content-Length", 0))
        raw_body = self.rfile.read(length).decode("utf-8") if length else "{}"

        try:
            data = json.loads(raw_body) if raw_body else {}
        except json.JSONDecodeError:
            self.send_json({"success": False, "error": "Invalid JSON request body"}, 400)
            return

        payload, status = handle_api("POST", path, data)
        self.send_json(payload, status)


if __name__ == "__main__":
    port = int(os.environ.get("PORT", "5000"))
    if Flask is not None:
        create_flask_app().run(host="127.0.0.1", port=port, debug=True)
    else:
        print(f"Flask is not installed. Running built-in fallback server at http://127.0.0.1:{port}")
        ThreadingHTTPServer(("127.0.0.1", port), FallbackHandler).serve_forever()
