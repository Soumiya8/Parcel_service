import json
import re
from http import HTTPStatus
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import parse_qs, unquote, urlparse


BASE_DIR = Path(__file__).resolve().parent
FRONTEND_DIR = BASE_DIR.parent / "Frontend"
DATA_FILE = BASE_DIR / "data.txt"


def read_data():
    parcels = []

    if not DATA_FILE.exists():
        return parcels

    with DATA_FILE.open("r", encoding="utf-8") as file:
        for line in file:
            parts = line.strip().split("|")

            if len(parts) >= 14:
                parcels.append({
                    "id": parts[0],
                    "sname": parts[1],
                    "saddr": parts[2],
                    "sphone": parts[3],
                    "rname": parts[4],
                    "raddr": parts[5],
                    "rphone": parts[6],
                    "status": parts[7],
                    "office": parts[8],
                    "date": parts[9],
                    "route": parts[10],
                    "driver": parts[11],
                    "priority": parts[12],
                    "failedAttempts": parts[13],
                })
            elif len(parts) >= 8:
                parcels.append({
                    "id": parts[0],
                    "sname": parts[1],
                    "saddr": parts[2],
                    "sphone": parts[3],
                    "rname": parts[4],
                    "raddr": parts[5],
                    "rphone": parts[6],
                    "status": parts[7],
                    "office": "Not Updated",
                    "date": "Not Updated",
                    "route": "General",
                    "driver": "Not Assigned",
                    "priority": "1",
                    "failedAttempts": "0",
                })

    return parcels


def write_data(parcels):
    with DATA_FILE.open("w", encoding="utf-8") as file:
        for parcel in parcels:
            file.write(
                f"{parcel['id']}|{parcel['sname']}|{parcel['saddr']}|{parcel['sphone']}|"
                f"{parcel['rname']}|{parcel['raddr']}|{parcel['rphone']}|{parcel['status']}|"
                f"{parcel['office']}|{parcel['date']}|{parcel['route']}|{parcel['driver']}|"
                f"{parcel['priority']}|{parcel['failedAttempts']}\n"
            )


def assign_driver(route, date):
    for parcel in read_data():
        if (
            parcel["route"].lower() == route.lower()
            and parcel["date"] == date
            and parcel["status"] != "Delivered"
        ):
            return parcel["driver"]

    route = route.upper()
    drivers = {
        "OMR": "Driver Arun",
        "ECR": "Driver Bala",
        "GST": "Driver Karthik",
        "PORUR": "Driver Naveen",
    }
    return drivers.get(route, "Driver Common Route")


def calculate_stats(parcels):
    delivered = sum(1 for parcel in parcels if parcel["status"] == "Delivered")
    urgent = sum(1 for parcel in parcels if parcel["priority"] == "2")
    failed = sum(1 for parcel in parcels if "Failed" in parcel["status"] or "Escalated" in parcel["status"])
    active = len(parcels) - delivered
    route_load = {}

    for parcel in parcels:
        route = parcel["route"] or "General"
        route_load[route] = route_load.get(route, 0) + 1

    busiest_route = "None"
    if route_load:
        busiest_route = max(route_load, key=route_load.get)

    return {
        "total": len(parcels),
        "active": active,
        "delivered": delivered,
        "urgent": urgent,
        "failed": failed,
        "busiestRoute": busiest_route,
    }


def validate_date(value):
    return bool(re.fullmatch(r"\d{2}-\d{2}-\d{4}", value))


def filter_parcels(parcels, query):
    search = query.get("search", [""])[0].strip().lower()
    location = query.get("location", [""])[0].strip().lower()
    date = query.get("date", [""])[0].strip()
    status = query.get("status", [""])[0].strip()
    priority = query.get("priority", [""])[0].strip()

    if search:
        parcels = [
            parcel for parcel in parcels
            if any(search in str(value).lower() for value in parcel.values())
        ]

    if location:
        parcels = [parcel for parcel in parcels if location in parcel["office"].lower()]

    if date:
        parcels = [parcel for parcel in parcels if parcel["date"] == date]

    if status:
        parcels = [parcel for parcel in parcels if parcel["status"] == status]

    if priority:
        parcels = [parcel for parcel in parcels if parcel["priority"] == priority]

    return parcels


class ParcelRequestHandler(SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        super().end_headers()

    def do_OPTIONS(self):
        self.send_response(HTTPStatus.NO_CONTENT)
        self.end_headers()

    def do_GET(self):
        parsed_url = urlparse(self.path)
        path = unquote(parsed_url.path)
        query = parse_qs(parsed_url.query)

        if path == "/":
            self.send_frontend_file("n.html")
        elif path == "/api":
            self.send_json({"message": "Parcel Service API is running"})
        elif path == "/parcels":
            self.send_json(filter_parcels(read_data(), query))
        elif path == "/stats":
            self.send_json(calculate_stats(read_data()))
        elif path.startswith("/track/"):
            tracking_id = path.removeprefix("/track/")
            self.track_parcel(tracking_id)
        else:
            self.send_frontend_file(path.lstrip("/"))

    def do_POST(self):
        path = unquote(urlparse(self.path).path)

        if path == "/add":
            self.add_parcel()
        elif path == "/update-status":
            self.update_status()
        else:
            self.send_json({"error": "Endpoint not found"}, HTTPStatus.NOT_FOUND)

    def do_DELETE(self):
        path = unquote(urlparse(self.path).path)

        if path.startswith("/parcel/"):
            tracking_id = path.removeprefix("/parcel/")
            self.delete_parcel(tracking_id)
        else:
            self.send_json({"error": "Endpoint not found"}, HTTPStatus.NOT_FOUND)

    def read_json_body(self):
        content_length = int(self.headers.get("Content-Length", 0))
        if content_length == 0:
            return {}

        raw_body = self.rfile.read(content_length).decode("utf-8")
        try:
            return json.loads(raw_body)
        except json.JSONDecodeError:
            return {}

    def send_json(self, payload, status=HTTPStatus.OK):
        body = json.dumps(payload).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def send_frontend_file(self, relative_path):
        file_path = (FRONTEND_DIR / relative_path).resolve()

        if not str(file_path).startswith(str(FRONTEND_DIR.resolve())) or not file_path.is_file():
            self.send_json({"error": "File not found"}, HTTPStatus.NOT_FOUND)
            return

        content_types = {
            ".css": "text/css",
            ".html": "text/html",
            ".js": "application/javascript",
        }
        content = file_path.read_bytes()
        self.send_response(HTTPStatus.OK)
        self.send_header("Content-Type", content_types.get(file_path.suffix, "application/octet-stream"))
        self.send_header("Content-Length", str(len(content)))
        self.end_headers()
        self.wfile.write(content)

    def track_parcel(self, tracking_id):
        for parcel in read_data():
            if parcel["id"] == tracking_id:
                self.send_json(parcel)
                return

        self.send_json({"error": "Parcel not found"}, HTTPStatus.NOT_FOUND)

    def add_parcel(self):
        data = self.read_json_body()
        required_fields = [
            "sname", "saddr", "sphone", "rname", "raddr", "rphone",
            "office", "date", "route", "priority",
        ]
        missing_fields = [field for field in required_fields if not data.get(field)]

        if missing_fields:
            self.send_json({
                "error": "Missing required fields",
                "fields": missing_fields,
            }, HTTPStatus.BAD_REQUEST)
            return

        if not validate_date(data["date"]):
            self.send_json({"error": "Delivery date must be in DD-MM-YYYY format"}, HTTPStatus.BAD_REQUEST)
            return

        if data["priority"] not in ["1", "2"]:
            self.send_json({"error": "Priority must be 1 or 2"}, HTTPStatus.BAD_REQUEST)
            return

        parcels = read_data()
        existing_ids = [int(parcel["id"]) for parcel in parcels if parcel["id"].isdigit()]
        new_id = max(existing_ids, default=999) + 1
        driver = assign_driver(data["route"], data["date"])

        parcel = {
            "id": str(new_id),
            "sname": data["sname"],
            "saddr": data["saddr"],
            "sphone": data["sphone"],
            "rname": data["rname"],
            "raddr": data["raddr"],
            "rphone": data["rphone"],
            "status": "Dispatched",
            "office": data["office"],
            "date": data["date"],
            "route": data["route"],
            "driver": driver,
            "priority": data["priority"],
            "failedAttempts": "0",
        }

        parcels.append(parcel)
        write_data(parcels)

        self.send_json({
            "message": "Parcel added successfully",
            "trackingID": new_id,
            "driver": driver,
        })

    def update_status(self):
        data = self.read_json_body()

        if not data.get("id") or not data.get("status"):
            self.send_json({"error": "Tracking ID and status are required"}, HTTPStatus.BAD_REQUEST)
            return

        parcels = read_data()

        for parcel in parcels:
            if parcel["id"] == data["id"]:
                if data["status"] == "Delivery Failed":
                    attempts = int(parcel["failedAttempts"]) + 1
                    parcel["failedAttempts"] = str(attempts)

                    if attempts == 1:
                        parcel["status"] = "Delivery Failed - Rescheduled"
                    elif attempts == 2:
                        parcel["status"] = "Urgent Attention Needed"
                        parcel["priority"] = "2"
                    else:
                        parcel["status"] = "Escalated to Office Manager"
                else:
                    parcel["status"] = data["status"]

                write_data(parcels)
                self.send_json({
                    "message": "Status updated successfully",
                    "parcel": parcel,
                })
                return

        self.send_json({"error": "Parcel not found"}, HTTPStatus.NOT_FOUND)

    def delete_parcel(self, tracking_id):
        parcels = read_data()
        updated_parcels = [parcel for parcel in parcels if parcel["id"] != tracking_id]

        if len(updated_parcels) == len(parcels):
            self.send_json({"error": "Parcel not found"}, HTTPStatus.NOT_FOUND)
            return

        write_data(updated_parcels)
        self.send_json({"message": "Parcel deleted successfully"})


if __name__ == "__main__":
    server = ThreadingHTTPServer(("127.0.0.1", 5000), ParcelRequestHandler)
    print("Parcel Service System running at http://127.0.0.1:5000")
    server.serve_forever()
