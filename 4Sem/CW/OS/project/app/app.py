from flask import Flask, request

app = Flask(__name__)

@app.route("/", defaults={"path": ""}, methods=["GET", "POST", "PUT", "DELETE", "PATCH"])
@app.route("/<path:path>", methods=["GET", "POST", "PUT", "DELETE", "PATCH"])
def catch_all(path):
    print(f"{request.method} /{path}", flush=True)
    for key, value in request.headers.items():
        print(f"{key}: {value}", flush=True)
    if request.data:
        print("BODY:", request.data.decode(errors="replace"), flush=True)
    return "MITM Flask responder\n", 200


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)