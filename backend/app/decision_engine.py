from backend.app.models import SensorReading, db

SAFE_TEMP = 30
SAFE_HUMIDITY = 70


def analyze_sensor_data(data):
    temperature = data.get('temperature')
    humidity = data.get('humidity')

    if temperature is None or humidity is None:
        return {'error': 'Missing sensor data'}

    reading = SensorReading(temperature=temperature, humidity=humidity)
    db.session.add(reading)
    db.session.commit()

    alert = None
    if temperature > SAFE_TEMP and humidity > SAFE_HUMIDITY:
        alert = 'High risk of mold growth'
    elif humidity > SAFE_HUMIDITY:
        alert = 'High humidity detected'
    elif temperature > SAFE_TEMP:
        alert = 'High temperature detected'

    return {
        'temperature': temperature,
        'humidity': humidity,
        'alert': alert,
        'recommended_action': 'Activate fan' if alert else 'All clear',
    }
