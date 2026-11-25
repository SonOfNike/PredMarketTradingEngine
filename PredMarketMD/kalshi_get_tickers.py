import requests

# Fetch all active moneyline markets for NFL games
url = "https://api.elections.kalshi.com/trade-api/v2/markets"
params = {"series_ticker": "KXNFLGAME", "status": "open"}

response = requests.get(url, params=params).json()

for market in response.get("markets", []):
    print(f"Ticker: {market['ticker']} | Title: {market['title']}")