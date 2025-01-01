import paramiko
import re

# Define server and login details
hostname = "192.168.0.15"
port = 22
username = "user"
password = "user"

# Create an SSH client
client = paramiko.SSHClient()
client.set_missing_host_key_policy(paramiko.AutoAddPolicy())

try:
    # Connect to the server
    client.connect(hostname, port, username, password)

    # Execute the command to source .bashrc and run the alias
    command = "bash -i -c 'source ~/.bashrc && pmaster && ls'"
    stdin, stdout, stderr = client.exec_command(command)
    print(f"Executing: {command}")

    output = stdout.read().decode()
    print(output)
    #print(stderr.read().decode())

    # Find "135806" in the output and store it as variable a
    match = re.search(r'135806', output)
    if match:
        a = match.group(0)
        print(f"Variable a: {a}")

    # Use the variable a in the next command
    command = f"bash -i -c 'source ~/.bashrc && pmaster && git pull origin master && bash build.sh --all'"
    stdin, stdout, stderr = client.exec_command(command)
    print(f"Executing: {command}")

    # Open a log file
    with open('output.log', 'w') as log_file:
        # Display build process in real-time and write to log file
        while True:
            line = stdout.readline()
            if not line:
                break
            print(line.strip())
            log_file.write(line)

        # Write stderr to log file if there's any
        stderr_output = stderr.read().decode()
        if stderr_output:
            print(stderr_output)
            log_file.write(stderr_output)

finally:
    # Close the connection
    client.close()
