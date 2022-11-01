import socket 


# Creating a socket 
serverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Assigning an IP address and port number to socket
serverSocket.bind(('192.168.1.100', 12000))

# Dropping first 20 messages due to invalid syntax
msg_counter = 20
for i in range(0, msg_counter):
    message, address = serverSocket.recvfrom(1024)


while True:
    message, address = serverSocket.recvfrom(1024)
    print(message.decode('utf-8').rstrip())
