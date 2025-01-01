import paramiko
import re
import time
import configparser
import pandas as pd

# Define server and login details
hostname = "192.168.150.3"
port = 3100
username = "user"
password = "user"  # Update this with the actual password

ANT_PORT_MAX = 1 # 4  # Replace with the actual number of antennas
rfic_config_init_remote_file_path = "/opt/o-ran/sw/sw-0/rfic_driver/RFIC_Config.ini"  # Update this with the actual path
rfic_config_ini_local_file_path = "RFIC_Config.ini"

rfic_client_local_file_path = "/home/user/projects/branches/master/pc805-mplane/gl805_oru-7.0.0/firmware/mp_sw/rfic_driver/AFE7769D_Driver_client"
rfic_server_local_file_path = "/home/user/projects/branches/master/pc805-mplane/gl805_oru-7.0.0/firmware/mp_sw/rfic_driver/AFE7769D_Driver_server"
rfic_client_remote_file_path = "/opt/o-ran/sw/sw-0/rfic_driver/AFE7769D_Driver_client.downloaded"
rfic_server_remote_file_path = "/opt/o-ran/sw/sw-0/rfic_driver/AFE7769D_Driver_server.downloaded"

get_commands = {
    1: "getTxDsa",
    2: "getRxDsa",
    3: "getRxRmsPower",
    4: "getTxPower",
    5: "getOverrideTddStatus",
    6: "getFbdsa",
    7: "get_dpd_dpdEstimInputGainVal",
    8: "get_dpd_cfr_gain_val",
    9: "get_dpd_dpdCorrOutputGainVal",
    10: "getDpdLoopTrackedDelay",
    11: "getDpdLoopChannelResponse",
    12: "getTxIqmcLolLoopTrackedDelay",
    13: "getCoreCfrDiagnosticParams",
    14: "getGlbVarDpdRead"
}

def read_rfic_config(shell, prompt, password=None):
    # Load the configuration file
    config = configparser.ConfigParser()
    config.read('RFIC_Config.ini')

    # Extract the variable values and store them in a dictionary
    variables = {}
    for section in config.sections():
        for key, value in config.items(section):
            variables[f"{section}.{key}"] = value

    # Convert the dictionary to a DataFrame for better presentation
    df = pd.DataFrame(list(variables.items()), columns=['Variable', 'Value'])

    # Display the DataFrame
    print(df.to_string(index=False))


# Create an SSH client
client = paramiko.SSHClient()
client.set_missing_host_key_policy(paramiko.AutoAddPolicy())

def wait_for_prompt(shell, prompt, password=None):
    buffer = ""
    while True:
        if shell.recv_ready():
            output = shell.recv(4096).decode()
            buffer += output
            print(output, end='')
            # Check for the sudo password prompt
            if password and re.search(r'\[sudo\] password for', buffer):
                shell.send(password + '\n')
            # Check for the specific prompt indicating readiness
            if re.search(prompt, buffer):
                break
        time.sleep(1)  # Wait for a second before checking again

def system_login():
    # Connect to the server using the username and password
    client.connect(hostname, port, username, password)

    # Open an interactive shell session
    shell = client.invoke_shell()
    time.sleep(1)  # Wait for the shell to be ready

    # Send the command to source .bashrc and run the alias
    command = "source ~/.bashrc\n"
    shell.send(command)

    return shell

def check_and_download_file(local_file_path, remote_file_path):
    sftp = client.open_sftp()
    try:
        sftp.stat(remote_file_path)
        print(f"{remote_file_path} exists. Downloading to local machine.")
        sftp.get(remote_file_path, local_file_path)
        print(f"Downloaded {remote_file_path} to {local_file_path}.")
    except FileNotFoundError:
        print(f"{remote_file_path} does not exist on the remote server.")
    finally:
        sftp.close()

def upload_file_to_remote(local_file_path, remote_file_path):
    sftp = client.open_sftp()
    try:
        sftp.put(local_file_path, remote_file_path)
        print(f"Uploaded {local_file_path} to {remote_file_path}.")
    except Exception as e:
        print(f"Failed to upload file: {e}")
    finally:
        sftp.close()

def file_handle_command(shell):

    # RFIC upload/download test:

    # RFIC_Config.ini
    #check_and_download_file(rfic_config_init_local_file_path, rfic_config_init_remote_file_path)
    #upload_file_to_remote(rfic_config_init_remote_file_path, rfic_config_init_local_file_path)

    # RFIC server/client
    #upload_file_to_remote(rfic_client_local_file_path, rfic_client_remote_file_path)
    #upload_file_to_remote(rfic_server_local_file_path, rfic_server_remote_file_path)

    print("File handling commands completed successfully.")

def rfic_login(shelf):
    # Send the command to source .bashrc and run the alias
    command = "crfic\n"
    shell.send(command)

    command = "sudo ./AFE7769D_Driver_client\n"
    shell.send(command)

    wait_for_prompt(shell, r'Password', password)
    shell.send('user\n')
    #wait_for_prompt(shell, r'$')

    # Wait for the initial prompt
    wait_for_prompt(shell, r'Please input msg type', password)

def rfic_execute_command(shell, mode, cmd, antenna_number):
    # Send the "read" command
    shell.send(mode)

    # Capture the output of the "read" command
    wait_for_prompt(shell, r'0 - Return to the parent menu')

    # Send the command to select the antenna
    shell.send(cmd)
    wait_for_prompt(shell, r'Input the')

    shell.send(f'{antenna_number}\n')
    wait_for_prompt(shell, r'Please input')

def rfic_handle(shell):
    rfic_login(shell)

    for antenna_number in range(ANT_PORT_MAX):
        # read
        print(f"Reading information for antenna {antenna_number}\n")
        #rfic_execute_command(shell,'read\n','3\n', antenna_number) 
        #rfic_execute_command(shell,'read\n','4\n', antenna_number) 
        #rfic_execute_command(shell,'read\n','5\n', antenna_number) 
        #rfic_execute_command(shell,'read\n','6\n', antenna_number) 

        # get 
        rfic_execute_command(shell,'get\n','1\n', antenna_number) 
        #rfic_execute_command(shell,'get\n','2\n', antenna_number) 
        #rfic_execute_command(shell,'get\n','3\n', antenna_number) 
        #rfic_execute_command(shell,'get\n','4\n', antenna_number) 
        #rfic_execute_command(shell,'get\n','7\n', antenna_number) 
        #rfic_execute_command(shell,'get\n','8\n', antenna_number) 
        #rfic_execute_command(shell,'get\n','9\n', antenna_number) 
        #rfic_execute_command(shell,'get\n','10\n', antenna_number) 
        #rfic_execute_command(shell,'get\n','11\n', antenna_number) 
        #rfic_execute_command(shell,'get\n','12\n', antenna_number) 
        #rfic_execute_command(shell,'get\n','13\n', antenna_number) 
        #rfic_execute_command(shell,'get\n','14\n', antenna_number) 
        print(f"Reading information for antenna {antenna_number} completed\n")

    shell.send('exit\n')
    wait_for_prompt(shell, r'Exit the program')

def test_tool_login(shelf):
    # Send the command to source .bashrc and run the alias
    command = "ctest\n"
    shell.send(command)

    # Wait for the initial prompt
    wait_for_prompt(shell, r'picocom_test_tool>>', password)

    # write TM3.1a 
    command = "write TM3.1a 0\n"
    shell.send(command)
    wait_for_prompt(shell, r'write done!', password)

def test_tool_logout(shelf):
    command = "exit\n"
    shell.send(command)
    wait_for_prompt(shell, r'$')
    
def test_tool_handle(shell):
    command = "sudo chmod 666 /dev/ttyPSC0.3\n"
    shell.send(command)
    wait_for_prompt(shell, r'Password', password)
    shell.send('user\n')
    wait_for_prompt(shell, r'$')

    test_tool_login(shell)

    test_tool_logout(shell)

try:
    # system login
    shell = system_login()

    # handling upload/download command
    file_handle_command(shell)

    # handling rfic command
    # rfic_handle(shell)

    # handling test_tool
    test_tool_handle(shell)

finally:
    # Close the connection
    client.close()
