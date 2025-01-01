import paramiko
import re
import time

# Define server and login details
hostname = "192.168.150.3"
port = 3100
username = "user"
password = "user"  # Update this with the actual password

# Create an SSH client
client = paramiko.SSHClient()
client.set_missing_host_key_policy(paramiko.AutoAddPolicy())

try:
    # Connect to the server using the username and password
    client.connect(hostname, port, username, password)

    # Open an interactive shell session
    shell = client.invoke_shell()
    time.sleep(1)  # Wait for the shell to be ready

    # Send the command to source .bashrc and run the alias
    command = "source ~/.bashrc && crfic && sudo ./AFE7769D_Driver_client\n"
    shell.send(command)
    print(f"Executing: {command}")

    # Wait until the prompt is received
    buffer = ""
    while True:
        if shell.recv_ready():
            output = shell.recv(4096).decode()
            buffer += output
            print(output, end='')
            # Check for the sudo password prompt
            if re.search(r'\[sudo\] password for', buffer):
                shell.send(password + '\n')
            # Check for the specific prompt indicating readiness
            if re.search(r'Please input msg type: read/get/set/load/save/exec/exit', buffer):
                break
        time.sleep(1)  # Wait for a second before checking again

    # Send the "read" command after the initial command execution
    shell.send('read\n')
    print("Sent 'read' command")

    # Capture the output of the "read" command
    buffer = ""
    while True:
        if shell.recv_ready():
            output = shell.recv(4096).decode()
            buffer += output
            print(output, end='')
            # Check for the prompt again to ensure the command has completed
            if re.search(r'Please input msg type: read/get/set/load/save/exec/exit', buffer):
                break
        time.sleep(1)  # Wait for a second before checking again

finally:
    # Close the connection
    client.close()
