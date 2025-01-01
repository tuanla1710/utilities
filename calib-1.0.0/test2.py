import paramiko
import time
import re
import logging

logging.basicConfig(level=logging.DEBUG)

hostname = "192.168.0.31"
port = 22
username = "user"
key_filename = "/home/user/.ssh/id_rsa"  # Path to your private key

# Custom host key policy to automatically add the host key
class AutoAddPolicy(paramiko.MissingHostKeyPolicy):
    def missing_host_key(self, client, hostname, key):
        print(f"Automatically adding host key for {hostname}")
        client._host_keys.add(hostname, key.get_name(), key)
        if client._host_keys_filename is None:
            client._host_keys_filename = "/home/user/.ssh/known_hosts"
        client._host_keys.save(client._host_keys_filename)

# Create an SSH client
client = paramiko.SSHClient()
client.set_missing_host_key_policy(AutoAddPolicy())

try:
    # Connect to the server using the username and private key
    client.connect(hostname, port, username, key_filename=key_filename)

    # Open an interactive shell session
    shell = client.invoke_shell()
    time.sleep(1)  # Wait for the shell to be ready

    # Send the command to source .bashrc and run the alias
    command = "source ~/.bashrc && crfic && ./AFE7769D_Driver_client\n"
    shell.send(command)
    print(f"Executing: {command}")

    # Wait until the prompt is received
    buffer = ""
    while True:
        if shell.recv_ready():
            output = shell.recv(1024).decode()
            buffer += output
            print(output, end='')
            if re.search(r'Please input msg type: read/get/set/load/save/exec/exit', buffer):
                break
        time.sleep(1)  # Wait for a second before checking again

    # Now you can proceed with further actions
    # For example, sending a command based on the prompt
    # shell.send('read\n')

finally:
    # Close the connection
    client.close()
