import pyvisa

# Connect to the instrument
rm = pyvisa.ResourceManager()
sa = rm.open_resource('TCPIP::192.168.0.104::INSTR')

# Increase timeout to 20 seconds
sa.timeout = 3000

# Set the instrument to remote mode
sa.write('SYST:REM')

# Set the mode to 5G NR
sa.write('INST:SEL 5GNR')

# Set the measurement to Channel Power
#sa.write('CONF:CHP')

# Set the view to Normal (if applicable, this might vary based on the instrument)
# sa.write('VIEW:NORM')  # Uncomment if this command is supported

# Send the command to get Total Channel Power in 5G NR 1 Channel Power
#sa.write('CALC:CHP:TOTAL?')

# Read the response from the instrument
response = sa.read()

# Print the response
print(f"Total Channel Power in 5G NR 1 Channel Power: {response}")
