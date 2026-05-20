from flask import request, jsonify
from backend.app import app
from backend.app.decision_engine import analyze_sensor_data

@app.route('/api/sensor', methods=['POST'])
def receive_sensor_data():
    data = request.get_json()
    if not data:
        return jsonify({'error': 'Invalid payload'}), 400

    result = analyze_sensor_data(data)
    return jsonify(result)

@app.route('/api/status', methods=['GET'])
def status():
    return jsonify({'status': 'ok'})
