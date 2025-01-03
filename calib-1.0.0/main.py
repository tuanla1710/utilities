import paramiko
import socket

def ssh_execute_command(ip, port, username, password, command):
    # Create an SSH client
    client = paramiko.SSHClient()
    # Automatically add the server's host key (not recommended for production)
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())

    try:
        # Connect to the server
        client.connect(ip, port=port, username=username, password=password)

        # Execute the command
        stdin, stdout, stderr = client.exec_command(command)

        # Read the output
        output = stdout.read().decode()
        error = stderr.read().decode()

        # Print the output and error (if any)
        print("Output:\n", output)
        if error:
            print("Error:\n", error)

    except socket.gaierror as e:
        print(f"Address-related error connecting to server: {e}")
    except paramiko.AuthenticationException:
        print("Authentication failed, please verify your credentials")
    except paramiko.SSHException as sshException:
        print(f"Unable to establish SSH connection: {sshException}")
    except Exception as e:
        print(f"Exception in connecting to the server: {e}")
    finally:
        # Close the connection
        client.close()

# Define the connection details
ip = "192.168.0.31"
port = 22
username = "user"
password = "user"

try:
    while True:
        # Prompt the user to enter a command
        command = input("Enter a command to execute on the remote server (or Ctrl+C to exit): ")

        # Combine commands if needed
        if 'cd' in command:
            command = f'bash -c "{command} && ls"'

        # Execute the command over SSH and display the output
        ssh_execute_command(ip, port, username, password, command)

except KeyboardInterrupt:
    print("\nExiting...")
