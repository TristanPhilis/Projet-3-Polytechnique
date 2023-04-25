import http.client
import sys
# BODY = "filecontents"
conn = http.client.HTTPConnection("localhost", 5000)

# conn.request("GET", "/scan")
# response = conn.getresponse()
# print(response.status, response.reason)

while True:
    message = \
    """Enter which command you want : 
    0 for identify
    1 for explore
    2 for land
    3 for information
    4 for takeoff
    q for quit
    """
    in_message = input(message).replace(' ', '')
    if in_message == 'q':
        sys.exit('You quit the app safely')
    elif in_message.isnumeric():
        url_ext = int(in_message)
        a = 0xE101CF0002
        if url_ext == 0:
            conn.request("GET", f"/identify/{a}")
        elif url_ext == 1:
            conn.request("GET", f"/explore/{a}")
        elif url_ext == 2:
            conn.request("GET", f"/land/{a}")
        elif url_ext == 3:
            conn.request("GET", f"/information/{a}")
        elif url_ext == 4:
            conn.request("GET", f"/takeoff/{a}")

        response = conn.getresponse()
        print(response.status, response.reason)