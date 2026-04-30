const credentials = {
  admin: { username: "admin", password: "1234" },
  user: { username: "user", password: "1111" },
};

const routeSuggestions = {
  chennai: ["OMR Chennai", "GST Road", "ECR Road"],
  coimbatore: ["Avinashi Road"],
  madurai: ["Ring Road"],
  trichy: ["Bypass Road"],
  salem: ["Five Roads"],
  tirunelveli: ["Tirunelveli Main Road"],
  bangalore: ["Electronic City Road"],
  kochi: ["MG Road"],
  hyderabad: ["Hitech City Road"],
};

const loginView = document.getElementById("loginView");
const adminView = document.getElementById("adminView");
const userView = document.getElementById("userView");
const rolePill = document.getElementById("rolePill");
const headerLogoutBtn = document.querySelector(".auth-area .logout-btn");
const loginAlertBox = document.getElementById("loginAlertBox");
const alertBox = document.getElementById("alertBox");
const userAlertBox = document.getElementById("userAlertBox");
const loader = document.getElementById("loader");
const recordsBody = document.getElementById("recordsBody");
const adminTrackResult = document.getElementById("adminTrackResult");
const userTrackResult = document.getElementById("userTrackResult");
const createMessage = document.getElementById("createMessage");
const destinationInput = document.getElementById("destinationInput");
const routeInput = document.getElementById("routeInput");
const routeOptions = document.getElementById("routeOptions");

let currentRole = "";

function getFormData(form) {
  return Object.fromEntries(new FormData(form).entries());
}

function escapeHtml(value) {
  return String(value ?? "").replace(/[&<>"']/g, (char) => ({
    "&": "&amp;",
    "<": "&lt;",
    ">": "&gt;",
    '"': "&quot;",
    "'": "&#039;",
  }[char]));
}

function activeAlertBox() {
  if (!currentRole) {
    return loginAlertBox;
  }
  return currentRole === "user" ? userAlertBox : alertBox;
}

function setLoading(isLoading) {
  loader.classList.toggle("hidden", !isLoading);
}

function showAlert(message, type = "success") {
  const box = activeAlertBox();
  box.textContent = message;
  box.className = `alert ${type}`;
}

function showCreateMessage(message, type = "success") {
  createMessage.textContent = message;
  createMessage.className = `form-message ${type}`;
}

function clearAlert() {
  [loginAlertBox, alertBox, userAlertBox].forEach((box) => {
    box.className = "alert hidden";
    box.textContent = "";
  });
}

function showError(error) {
  const fields = Array.isArray(error.fields) ? `: ${error.fields.join(", ")}` : "";
  showAlert(`${error.error || "Request failed"}${fields}`, "error");
}

function statusClass(status) {
  if (status === "Delivered") {
    return "delivered";
  }
  if (status === "Emergency Escalation") {
    return "emergency";
  }
  if (status === "Office Hold" || String(status).includes("Failed")) {
    return "danger";
  }
  if (String(status).includes("Out") || String(status).includes("Transit")) {
    return "warning";
  }
  return "";
}

function statusBadge(status) {
  return `<span class="badge ${statusClass(status)}">${escapeHtml(status)}</span>`;
}

async function api(path, options = {}) {
  setLoading(true);
  clearAlert();

  try {
    const response = await fetch(path, options);
    const text = await response.text();
    let payload;

    try {
      payload = text ? JSON.parse(text) : {};
    } catch {
      throw { error: "Server returned invalid JSON", details: text };
    }

    if (!response.ok || payload.success === false) {
      console.log("API error response", path, payload);
      throw payload;
    }

    console.log("API response", path, payload);
    return payload;
  } finally {
    setLoading(false);
  }
}

function login(role, form) {
  const data = getFormData(form);
  const expected = credentials[role];

  if (data.username === expected.username && data.password === expected.password) {
    currentRole = role;
    loginView.classList.add("hidden");
    adminView.classList.toggle("hidden", role !== "admin");
    userView.classList.toggle("hidden", role !== "user");
    rolePill.textContent = role === "admin" ? "Admin logged in" : "User logged in";
    headerLogoutBtn.classList.remove("hidden");
    form.reset();
    clearAlert();

    if (role === "admin") {
      refreshAll("Welcome admin. Dashboard loaded.");
    } else {
      showAlert("Welcome user. Enter your tracking ID.", "success");
    }
    return;
  }

  currentRole = "";
  showAlert("Invalid username or password.", "error");
}

function logout() {
  currentRole = "";
  loginView.classList.remove("hidden");
  adminView.classList.add("hidden");
  userView.classList.add("hidden");
  rolePill.textContent = "Login required";
  headerLogoutBtn.classList.add("hidden");
  clearAlert();
  createMessage.className = "form-message hidden";
  createMessage.textContent = "";
  adminTrackResult.className = "parcel-card muted-card";
  adminTrackResult.textContent = "Track a parcel to view timeline details.";
  userTrackResult.className = "parcel-card muted-card";
  userTrackResult.textContent = "Your parcel details will appear here.";
}

function renderStats(stats = {}) {
  document.getElementById("statTotal").textContent = stats.total || 0;
  document.getElementById("statPending").textContent = stats.active || 0;
  document.getElementById("statDelivered").textContent = stats.delivered || 0;
  document.getElementById("statFailed").textContent = stats.failed || 0;
  document.getElementById("statEmergency").textContent = stats.emergencyEscalations || 0;
}

function renderParcels(parcels = []) {
  recordsBody.innerHTML = "";

  if (parcels.length === 0) {
    recordsBody.innerHTML = `<tr><td colspan="8">No delivery records found.</td></tr>`;
    return;
  }

  for (const parcel of parcels) {
    const row = document.createElement("tr");
    row.innerHTML = `
      <td>${escapeHtml(parcel.trackingID)}</td>
      <td>${escapeHtml(parcel.senderName)}</td>
      <td>${escapeHtml(parcel.receiverName)}</td>
      <td>${statusBadge(parcel.status)}</td>
      <td>${escapeHtml(parcel.destinationOffice)}</td>
      <td>${escapeHtml(parcel.deliveryDate)}</td>
      <td>${escapeHtml(parcel.roadRoute)}</td>
      <td>${escapeHtml(parcel.driverName)}</td>
    `;
    recordsBody.appendChild(row);
  }
}

function timelineClass(parcel, label) {
  const status = parcel.status;
  if (status === "Emergency Escalation" || status === "Office Hold" || status.includes("Failed")) {
    return label.includes("Failed") || label.includes("Emergency") ? "timeline-step danger" : "timeline-step done";
  }

  const order = ["Dispatched", "In Transit", "Reached Destination Office", "Out for Delivery", "Delivered"];
  const currentIndex = order.indexOf(status);
  const labelIndex = order.indexOf(label);

  if (labelIndex < currentIndex || status === "Delivered") {
    return "timeline-step done";
  }
  if (labelIndex === currentIndex) {
    return "timeline-step current";
  }
  return "timeline-step";
}

function renderTimeline(parcel) {
  const steps = [
    "Dispatched",
    "In Transit",
    "Reached Destination Office",
    "Out for Delivery",
    "Delivered",
  ];

  if (parcel.status.includes("Failed") || parcel.status === "Office Hold" || parcel.status === "Emergency Escalation") {
    steps.push(parcel.status === "Emergency Escalation" ? "Emergency Escalation" : "Failed Delivery Review");
  }

  return `
    <div class="timeline">
      ${steps.map((step) => `
        <div class="${timelineClass(parcel, step)}">
          <span class="timeline-dot"></span>
          <span>${escapeHtml(step)}</span>
        </div>
      `).join("")}
    </div>
  `;
}

function renderTrackedParcel(parcel, target) {
  target.className = "parcel-card";
  target.innerHTML = `
    <dl>
      <dt>ID</dt><dd>${escapeHtml(parcel.trackingID)}</dd>
      <dt>Sender</dt><dd>${escapeHtml(parcel.senderName)}</dd>
      <dt>Receiver</dt><dd>${escapeHtml(parcel.receiverName)}</dd>
      <dt>Status</dt><dd>${statusBadge(parcel.status)}</dd>
      <dt>Driver</dt><dd>${escapeHtml(parcel.driverName)}</dd>
      <dt>Route</dt><dd>${escapeHtml(parcel.roadRoute)}</dd>
      <dt>Office</dt><dd>${escapeHtml(parcel.destinationOffice)}</dd>
      <dt>Date</dt><dd>${escapeHtml(parcel.deliveryDate)}</dd>
    </dl>
    ${renderTimeline(parcel)}
  `;
}

function updateRouteSuggestions() {
  const city = destinationInput.value.trim().toLowerCase();
  const suggestions = routeSuggestions[city] || [];

  routeOptions.innerHTML = "";
  const allRoutes = new Set([
    ...suggestions,
    "OMR Chennai",
    "GST Road",
    "ECR Road",
    "Avinashi Road",
    "Ring Road",
    "Bypass Road",
    "Five Roads",
    "Electronic City Road",
    "MG Road",
    "Hitech City Road",
    "Tirunelveli Main Road",
  ]);

  for (const route of allRoutes) {
    const option = document.createElement("option");
    option.value = route;
    routeOptions.appendChild(option);
  }

  if (suggestions.length > 0 && (!routeInput.value || !allRoutes.has(routeInput.value))) {
    routeInput.value = suggestions[0];
  }
}

async function refreshAll(message = "") {
  if (currentRole !== "admin") {
    return;
  }

  try {
    const statsPayload = await api("/api/stats");
    const listPayload = await api("/api/list");
    renderStats(statsPayload.stats);
    renderParcels(listPayload.parcels);
    if (message) {
      showAlert(message);
    }
  } catch (error) {
    showError(error);
  }
}

async function postJson(path, data) {
  const payload = { ...data };
  console.log(payload);
  return api(path, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(payload),
  });
}

async function trackFromForm(form, target) {
  const { trackingID } = getFormData(form);

  try {
    const payload = await api(`/api/track/${encodeURIComponent(trackingID.trim())}`);
    renderTrackedParcel(payload.parcel, target);
    showAlert(`Parcel ${payload.parcel.trackingID} loaded.`);
  } catch (error) {
    target.className = "parcel-card muted-card";
    target.textContent = "Parcel not found.";
    showError(error);
  }
}

document.getElementById("adminLoginForm").addEventListener("submit", (event) => {
  event.preventDefault();
  login("admin", event.currentTarget);
});

document.getElementById("userLoginForm").addEventListener("submit", (event) => {
  event.preventDefault();
  login("user", event.currentTarget);
});

document.querySelectorAll(".logout-btn").forEach((button) => {
  button.addEventListener("click", logout);
});

destinationInput.addEventListener("input", updateRouteSuggestions);
destinationInput.addEventListener("change", updateRouteSuggestions);
updateRouteSuggestions();

async function addParcel(event) {
  event.preventDefault();
  const form = event.currentTarget;
  createMessage.className = "form-message hidden";
  createMessage.textContent = "";
  const formValues = getFormData(form);
  const payload = {
    senderName: formValues.senderName,
    senderAddress: formValues.senderAddress,
    senderContact: formValues.senderContact,
    receiverName: formValues.receiverName,
    receiverAddress: formValues.receiverAddress,
    receiverContact: formValues.receiverContact,
    destinationOffice: formValues.destinationOffice,
    deliveryDate: formValues.deliveryDate,
    roadRoute: formValues.roadRoute,
    priority: formValues.priority,
  };

  try {
    const data = await postJson("/api/add", payload);
    console.log(data);
    form.reset();
    updateRouteSuggestions();
    renderTrackedParcel(data.parcel, adminTrackResult);
    await refreshAll(`Parcel ${data.trackingID} created and assigned to ${data.parcel.driverName}.`);
    showCreateMessage(`Parcel ${data.trackingID} created successfully.`, "success");
  } catch (error) {
    console.log("Create Parcel failed", error);
    showCreateMessage(error.error || "Parcel creation failed.", "error");
    showError(error);
  }
}

window.addParcel = addParcel;

document.getElementById("adminTrackForm").addEventListener("submit", async (event) => {
  event.preventDefault();
  await trackFromForm(event.currentTarget, adminTrackResult);
});

document.getElementById("userTrackForm").addEventListener("submit", async (event) => {
  event.preventDefault();
  await trackFromForm(event.currentTarget, userTrackResult);
});

document.getElementById("updateForm").addEventListener("submit", async (event) => {
  event.preventDefault();
  const form = event.currentTarget;

  try {
    const payload = await postJson("/api/update", getFormData(form));
    form.reset();
    renderTrackedParcel(payload.parcel, adminTrackResult);
    await refreshAll(`Parcel ${payload.parcel.trackingID} status updated.`);
  } catch (error) {
    showError(error);
  }
});

document.getElementById("failedForm").addEventListener("submit", async (event) => {
  event.preventDefault();
  const form = event.currentTarget;
  const { trackingID } = getFormData(form);

  try {
    const payload = await api(`/api/failed/${encodeURIComponent(trackingID.trim())}`, { method: "POST" });
    form.reset();
    renderTrackedParcel(payload.parcel, adminTrackResult);
    await refreshAll(`Failed attempt recorded. New status: ${payload.parcel.status}.`);
  } catch (error) {
    showError(error);
  }
});

document.getElementById("locationForm").addEventListener("submit", async (event) => {
  event.preventDefault();
  const { location } = getFormData(event.currentTarget);

  try {
    const payload = await api(`/api/filter/location/${encodeURIComponent(location.trim())}`);
    renderParcels(payload.parcels);
    showAlert(`${payload.count} record(s) found for ${location}.`);
  } catch (error) {
    showError(error);
  }
});

document.getElementById("dateForm").addEventListener("submit", async (event) => {
  event.preventDefault();
  const { date } = getFormData(event.currentTarget);

  try {
    const payload = await api(`/api/filter/date/${encodeURIComponent(date.trim())}`);
    renderParcels(payload.parcels);
    showAlert(`${payload.count} record(s) found for ${date}.`);
  } catch (error) {
    showError(error);
  }
});

document.getElementById("refreshBtn").addEventListener("click", () => refreshAll("Dashboard refreshed."));
document.getElementById("showAllBtn").addEventListener("click", () => refreshAll("Showing all delivery records."));

headerLogoutBtn.classList.add("hidden");
