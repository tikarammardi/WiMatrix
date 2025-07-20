import os


Import("env")


wifi_ssid = os.environ.get('WIFI_SSID', 'DefaultSSID')
wifi_password = os.environ.get('WIFI_PASSWORD', 'DefaultPassword')


print(f"Loading WiFi credentials:")
print(f"SSID: {wifi_ssid}")
print(f"Password length: {len(wifi_password)}")

# Add build flags with proper escaping
env.Append(CPPDEFINES=[
    ("WIFI_SSID", f'\\"{wifi_ssid}\\"'),
    ("WIFI_PASSWORD", f'\\"{wifi_password}\\"')
])
