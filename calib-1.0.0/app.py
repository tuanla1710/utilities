from flask import Flask, render_template, request, redirect, url_for, session
import paramiko

app = Flask(__name__)
app.secret_key = 'your_secret_key'

@app.route('/')
def index():
    return render_template('login.html')

@app.route('/login', methods=['POST'])
def login():
    session['ip'] = request.form['ip']
    session['port'] = int(request.form['port'])
    session['username'] = request.form['username']
    session['password'] = request.form['password']
    return redirect(url_for('command'))

@app.route('/command', methods=['GET', 'POST'])
def command():
    if request.method == 'POST':
        command = request.form['command']
        output = ssh_execute_command(session['ip'], session['port'], session['username'], session['password'], command)
        return render_template('command.html', output=output)
    return render_template('command.html')

def ssh_execute_command(ip, port, username, password, command):
    client = paramiko.SSHClient()
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    try:
        client.connect(ip, port=port, username=username, password=password)
        stdin, stdout, stderr = client.exec_command(command)
        output = stdout.read().decode()
        error = stderr.read().decode()
        if error:
            output += "\nError:\n" + error
        return output
    except Exception as e:
        return str(e)
    finally:
        client.close()

if __name__ == '__main__':
    app.run(host='192.168.150.15', port=5000, debug=True)
    #app.run(debug=True)

