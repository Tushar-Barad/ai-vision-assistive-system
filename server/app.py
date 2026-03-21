from flask import Flask, request, jsonify, render_template
import requests
import base64
import json
from datetime import datetime
import re

app = Flask(__name__)

# Store the latest data
latest_data = {
    "description": "",
    "image": "",
    "timestamp": None
}

# Google Apps Script endpoint
GOOGLE_SCRIPT_URL = "https://script.google.com/macros/s/AKfycbxK1N-BGYsDmGjJokGWuyIY1qkjLodPqSa3Zg3T0GQYwiDvh7Zj4zYvlIH9oryt2cW5CQ/exec"

# Language detection functions
def contains_devanagari(text):
    # Check for Hindi (Devanagari script)
    return bool(re.search('[\u0900-\u097F]', text))

def contains_gujarati(text):
    # Check for Gujarati script
    return bool(re.search('[\u0A80-\u0AFF]', text))

@app.route("/")
def home():
    return render_template('index.html')  # Serve the HTML interface

@app.route("/get-latest", methods=["GET"])
def get_latest():
    return jsonify(latest_data)

@app.route("/post", methods=["POST"])
def post_data():
    try:
        # Check if this is text or image data
        content_type = request.content_type
        
        if 'application/json' in content_type:
            # JSON data (with both text and image)
            data = request.get_json()
            
            if "description" in data:
                latest_data["description"] = data["description"]
                
                # Detect language
                if contains_devanagari(data["description"]):
                    latest_data["language"] = "hi"
                elif contains_gujarati(data["description"]):
                    latest_data["language"] = "gu"
                else:
                    latest_data["language"] = "en"
                
            if "image" in data:
                latest_data["image"] = data["image"]
                
            latest_data["timestamp"] = datetime.now().isoformat()
            
            # Send to Google Apps Script
            try:
                r = requests.post(GOOGLE_SCRIPT_URL, json=data)
                return jsonify({
                    "status": "success",
                    "google_response": r.text
                })
            except Exception as e:
                return jsonify({"error": str(e)}), 500
                
        else:
            return jsonify({"error": "Unsupported content type"}), 400
            
    except Exception as e:
        return jsonify({"error": str(e)}), 400

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)