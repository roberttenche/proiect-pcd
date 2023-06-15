import os
import subprocess

class File:
  def __init__(self):
    self.path = ""
    self.blur_multiplier = 0
    self.upscale_multiplier = 0
    self.downscale_multiplier = 0

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
        print("You selected Admin")
        # Add your code for Option 2 here
    elif choice == 3:
        print("Have a nice day!")
        exit(0)
    else:
        print("Invalid choice")

    os.system("clear")