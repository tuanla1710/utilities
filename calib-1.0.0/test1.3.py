import paramiko
import re
import time
import configparser
import pandas as pd
import logging

# Define server and login details
hostname = "192.168.150.3"
#port = 3200
port = 3100
username = "user"
password = "user"  # Update this with the actual password

ANT_PORT_MAX = 1 # 4  # Replace with the actual number of antennas
DPD_MODEL_MAX = 5000 
DPD_TRACK_TIMEOUT = 5

rfic_config_init_remote_file_path = "/opt/o-ran/sw/sw-0/rfic_driver/RFIC_Config.ini"  # Update this with the actual path
rfic_config_ini_local_file_path = "RFIC_Config.ini"

rfic_client_local_file_path = "/home/user/projects/branches/master/pc805-mplane/gl805_oru-7.0.0/firmware/mp_sw/rfic_driver/AFE7769D_Driver_client"
rfic_server_local_file_path = "/home/user/projects/branches/master/pc805-mplane/gl805_oru-7.0.0/firmware/mp_sw/rfic_driver/AFE7769D_Driver_server"
rfic_client_remote_file_path = "/opt/o-ran/sw/sw-0/rfic_driver/AFE7769D_Driver_client.downloaded"
rfic_server_remote_file_path = "/opt/o-ran/sw/sw-0/rfic_driver/AFE7769D_Driver_server.downloaded"


# Configure logging
logging.basicConfig(filename='app.log', level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')


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
        #time.sleep(1)  # Wait for a second before checking again

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

    #wait_for_prompt(shell, r'Password', password)
    #shell.send('user\n')
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

def rfic_logout(shell):
    shell.send('exit\n')
    wait_for_prompt(shell, r'Exit the program')

def rfic_getall(shell):
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

def wait_for_prompt_dpd_model(shell, prompt, password=None, timeout=3):
    buffer = ""
    start_time = time.time()
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
                return True;

        # Check for timeout
        if time.time() - start_time > timeout:
            return False;

        time.sleep(1)  # Wait for a second before checking again
        
def rfic_set_dpd_model_command(shell, model):
    shell.send('set\n')
    wait_for_prompt(shell, r'0 - Return to the parent menu')

    shell.send('7\n')
    wait_for_prompt(shell, r'Input the')

    shell.send('0\n')
    wait_for_prompt(shell, r'Input the')

    shell.send(f'{model}\n')
    wait_for_prompt(shell, r'Please input')

    shell.send('exec\n')
    wait_for_prompt(shell, r'0 - Return to the parent menu')

    shell.send('7\n')
    wait_for_prompt(shell, r'Please input')
    
    shell.send('exec\n')
    wait_for_prompt(shell, r'0 - Return to the parent menu')

    shell.send('8\n')
    wait_for_prompt(shell, r'Input the')
    
    shell.send('0x1\n')
    wait_for_prompt(shell, r'Input the')
    
    shell.send('1\n')
    wait_for_prompt(shell, r'Please input')

    time.sleep(5)  # Wait for 5 second before checking again

def rfic_dpd_tracking_confirm(shell, model):
    dpd_tracking = False

    rfic_set_dpd_model_command(shell, model) 

    for check in range(1, DPD_TRACK_TIMEOUT):
        shell.send('read\n')
        wait_for_prompt(shell, r'0 - Return to the parent menu')
        shell.send('6\n')
        wait_for_prompt(shell, r'Input the')
        shell.send(f'0\n')
        dpd_tracking = wait_for_prompt_dpd_model(shell, r'dpd state = 0x12', 3)
        if dpd_tracking:
            return True
    return False

def dpd_model_lookup(shell):
    try:
        with open('gsearch.txt', 'r') as file:
            gsearch = int(file.read().strip())
    except FileNotFoundError:
        gsearch = 0    

    for model in range(gsearch, DPD_MODEL_MAX + 1):
        ret = rfic_dpd_tracking_confirm(shell, model)
        if ret:
            logging.info(f'[dpd_state = 0x12] with model = {model}')

        with open('gsearch.txt', 'w') as file:
            file.write(str(model))            
    
def dpd_model_set(shell, model):
    ret = rfic_dpd_tracking_confirm(shell, model)


def rfic_handle(shell):
    rfic_login(shell)

    #rfic_getall(shell)
    #dpd_model_lookup(shell)        

    rfic_dpd_tracking_confirm(shell, 20)

    rfic_logout(shell)

def test_tool_login(shelf):
    # Send the command to source .bashrc and run the alias
    command = "ctest\n"
    shell.send(command)

    # Wait for the initial prompt
    wait_for_prompt(shell, r'picocom_test_tool>>', password)

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

    command = "stop\n"
    shell.send(command)
    wait_for_prompt(shell, r'process STOP_REQ done', password)

    # write TM3.1a 
    command = "write TM3.1a 0\n"
    shell.send(command)
    wait_for_prompt(shell, r'write done!', password)

    command = "start 0 0\n"
    shell.send(command)
    wait_for_prompt(shell, r'process START_REQ done', password)

    test_tool_logout(shell)

try:
    # system login
    shell = system_login()

    # handling upload/download command
    #file_handle_command(shell)

    # handling test_tool
    test_tool_handle(shell)

    # handling rfic command
    rfic_handle(shell)


finally:
    # Close the connection
    client.close()
