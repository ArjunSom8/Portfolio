from flask import Flask, jsonify

app = Flask(__name__)

@app.route("/health")
def health():
    # Endpoint used by Kubernetes liveness/readiness probes
    return jsonify(status="ok"), 200

@app.route("/")
def home():
    # Simple root route for sanity checks and basic metrics
    return jsonify(message="Hello from microservice"), 200

if __name__ == "__main__":
    # Listen on all interfaces so Kubernetes can reach it
    app.run(host="0.0.0.0", port=8080)