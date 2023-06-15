import sys
import os
import socket
import struct
import stat

SERVER_IP = "192.168.174.48"
SERVER_PORT = 42069
MAX_LINE_SIZE = 1024

file_path = ""
apply_blur = 0
apply_upscale = 0
apply_downscale = 0


def get_file_extension(file_name):
  _, extension = os.path.splitext(file_name)
  return extension[1:]

for i in range(1, len(sys.argv)):
  if sys.argv[i] == "-f" or sys.argv[i] == "--file":
    file_path = sys.argv[i + 1]
  elif sys.argv[i] == "--blur":
    apply_blur = int(sys.argv[i + 1])
  elif sys.argv[i] == "--upscale":
    apply_upscale = int(sys.argv[i + 1])
  elif sys.argv[i] == "--downscale":
    apply_downscale = int(sys.argv[i + 1])

if file_path is None:
  print("[ERROR]: No input file specified. Use -f or --file.")
  sys.exit(1)

try:
  file_fd = os.open(file_path, os.O_RDONLY)
except OSError as e:
  print("[ERROR]: File not accessible:", file_path)
  sys.exit(1)

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
  client_socket.connect((SERVER_IP, SERVER_PORT))
except:
  print("[ERROR]: Connection refused.")
  sys.exit(1)

file_stat = os.fstat(file_fd)

header = struct.pack('20sNBBBxxxxx',
  get_file_extension(file_path).encode(),
  file_stat.st_size,
  apply_blur,
  apply_upscale,
  apply_downscale
)

client_socket.send(header)

# SEND
while True:

  response = bytearray(6)

  buffer = os.read(file_fd, MAX_LINE_SIZE)
  read_count = len(buffer)

  if read_count == 0:
    break

  client_socket.send(buffer)
  
  client_socket.recv_into(response)

  response = response.decode()

  while bytes(response.encode()) != b'SV_OK\x00':
    client_socket.send(buffer)
    client_socket.recv_into(response)
    response = response.decode()

client_socket.send(b"CL_TR_DONE\x00")


# RECEIVE
while True:

  buffer = client_socket.recv(MAX_LINE_SIZE)
  if not buffer:
    break

  new_name = "processed_" + os.path.basename(file_path)
  with open(new_name, "ab") as output_file:
    output_file.write(buffer)

  client_socket.send(b"CL_OK\x00")

client_socket.close()
os.close(file_fd)

sys.exit(0)
