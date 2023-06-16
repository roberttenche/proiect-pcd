import os
import socket
import time

SERVER_IP = "192.168.174.48"
SERVER_PORT = 42069
MAX_LINE_SIZE = 1024

class File:
  def __init__(self):
    self.path = ""
    self.blur_multiplier = 0
    self.upscale_multiplier = 0
    self.downscale_multiplier = 0

def admin_shell():

  file_idx = -1
  files = [File() for _ in range(50)]

  os.system("clear")
  print("Admin mode")

  username = input("Username: ")
  password = input("Password: ")

  if len(username) <= 1 or len(password) <= 1:
    return -1

  token = ""

  server_addr = (SERVER_IP, SERVER_PORT)
  client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

  try:
    client_socket.connect(server_addr)
  except:
    print("Error creating socket")
    return -1

  auth_str = f"{username} {password}"
  client_socket.sendto(auth_str.encode(), server_addr)

  token, _ = client_socket.recvfrom(18)
  token = token.decode()[:-1]

  if token == "Occupied\x00":
    print("Admin already connected")
    time.sleep(3)
    return 1
  elif token == "Wrong credentials\x00":
    print("Wrong credentials")
    time.sleep(3)
    return 1
  elif not token:
    print("Failed connection to server\x00")
    time.sleep(3)
    return 1

  while True:
    valid_command = False
    admin_command = False
    command = input(">>> ")

    # HIST
    if command.startswith("hist"):
      auth_command = f"{token} {command}\x00"
      client_socket.sendto(auth_command.encode(), server_addr)

      log_size_bytes, _ = client_socket.recvfrom(8)
      log_size = int.from_bytes(log_size_bytes, "little")

      file_buffer, _ = client_socket.recvfrom(log_size)
      print(file_buffer.decode())

      admin_command = True
      valid_command = True

    # CLEAR
    elif command.strip() == "clear":
      os.system("clear")

      admin_command = True
      valid_command = True

    # CLEAR LOGS
    elif command.strip() == "clear logs":
      auth_command = f"{token} {command}"
      client_socket.sendto(auth_command.encode(), server_addr)

      admin_command = True
      valid_command = True

    # CLEAR FILES
    elif command.strip() == "clear files":
      auth_command = f"{token} {command}"
      client_socket.sendto(auth_command.encode(), server_addr)

      admin_command = True
      valid_command = True

    # CONN
    elif command.startswith("conn"):
      auth_command = f"{token} {command}"
      client_socket.sendto(auth_command.encode(), server_addr)

      conn_count_bytes, _ = client_socket.recvfrom(4)
      conn_count = int.from_bytes(conn_count_bytes, "little")

      for _ in range(conn_count):
        conn_buffer, _ = client_socket.recvfrom(1024)
        print(conn_buffer.decode())

      admin_command = True
      valid_command = True

    # KICK
    elif command.startswith("kick"):
      auth_command = f"{token} {command}"
      client_socket.sendto(auth_command.encode(), server_addr)

      admin_command = True
      valid_command = True

    # BAN
    elif command.startswith("ban"):
      auth_command = f"{token} {command}"
      client_socket.sendto(auth_command.encode(), server_addr)

      admin_command = True
      valid_command = True

    # FILE
    if command.startswith("file"):
      file_idx += 1
      file_path = command.split(" ", 1)[1]
      files[file_idx].path = file_path
      valid_command = True

    # BLUR
    elif command.startswith("blur") and file_idx != -1:
      blur_amount = command.split(" ", 1)[1]
      files[file_idx].blur_multiplier = int(blur_amount)
      valid_command = True

    # UPSCALE
    elif command.startswith("upscale") and file_idx != -1:
      upscale_amount = command.split(" ", 1)[1]
      files[file_idx].upscale_multiplier = int(upscale_amount)
      valid_command = True

    # DOWNSCALE
    elif command.startswith("downscale") and file_idx != -1:
      downscale_amount = command.split(" ", 1)[1]
      files[file_idx].downscale_multiplier = int(downscale_amount)
      valid_command = True

    # LIST
    elif command.startswith("list") and file_idx != -1:
      for i in range(file_idx + 1):
        f = files[i]
        print(f"file: '{f.path}' blur: '{f.blur_multiplier}' upscale: '{f.upscale_multiplier}' downscale: '{f.downscale_multiplier}'")
      valid_command = True

    # EXECUTE
    elif command.startswith("execute") and file_idx != -1:
      for i in range(file_idx + 1):
        f = files[i]
        blur_str = str(f.blur_multiplier)
        upscale_str = str(f.upscale_multiplier)
        downscale_str = str(f.downscale_multiplier)

        try:
          os.system(
            f"python python_user.py --file {f.path} --blur {blur_str} --upscale {upscale_str} --downscale {downscale_str}"
          )
        except:
            print("execute error")
      valid_command = True

    # EXIT
    elif command.startswith("exit"):
      auth_command = f"{token} {command}"
      client_socket.sendto(auth_command.encode(), server_addr)
      return 0

    if not valid_command and file_idx == -1 and not admin_command:
      print("Use 'file' command first.")
    elif not valid_command:
      print(f"Unrecognized command: '{command}'")

def user_shell():
  file_idx = -1
  files = [File() for _ in range(50)]

  os.system("clear")
  print("User mode")

  while True:
    valid_command = False
    command = input(">>> ")

    # FILE
    if command.startswith("file"):
      file_idx += 1
      file_path = command.split(" ", 1)[1]
      files[file_idx].path = file_path
      valid_command = True

    # BLUR
    elif command.startswith("blur") and file_idx != -1:
      blur_amount = command.split(" ", 1)[1]
      files[file_idx].blur_multiplier = int(blur_amount)
      valid_command = True

    # UPSCALE
    elif command.startswith("upscale") and file_idx != -1:
      upscale_amount = command.split(" ", 1)[1]
      files[file_idx].upscale_multiplier = int(upscale_amount)
      valid_command = True

    # DOWNSCALE
    elif command.startswith("downscale") and file_idx != -1:
      downscale_amount = command.split(" ", 1)[1]
      files[file_idx].downscale_multiplier = int(downscale_amount)
      valid_command = True

    # LIST
    elif command.startswith("list") and file_idx != -1:
      for i in range(file_idx + 1):
        f = files[i]
        print(f"file: '{f.path}' blur: '{f.blur_multiplier}' upscale: '{f.upscale_multiplier}' downscale: '{f.downscale_multiplier}'")
      valid_command = True

    # EXECUTE
    elif command.startswith("execute") and file_idx != -1:
      for i in range(file_idx + 1):
        f = files[i]
        blur_str = str(f.blur_multiplier)
        upscale_str = str(f.upscale_multiplier)
        downscale_str = str(f.downscale_multiplier)

        try:
          os.system(
            f"python python_user.py --file {f.path} --blur {blur_str} --upscale {upscale_str} --downscale {downscale_str}"
          )
        except:
            print("execute error")
      valid_command = True

    # EXIT
    elif command.startswith("exit"):
      return 0

    if not valid_command and file_idx == -1:
      print("Use 'file' command first.")
    elif not valid_command:
      print(f"Unrecognized command: '{command}'")


### BEGIN ###

os.system("clear")
choice = 0

while True:
    print("Main Menu")
    print("1. User")
    print("2. Admin")
    print("3. Exit")
    choice = int(input("Enter your choice (1-3): "))

    if choice == 1:
        user_shell()
    elif choice == 2:
        admin_shell()
    elif choice == 3:
        print("Have a nice day!")
        exit(0)
    else:
        print("Invalid choice")

    os.system("clear")