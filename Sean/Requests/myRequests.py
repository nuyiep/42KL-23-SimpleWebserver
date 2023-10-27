import requests
import sys

print('Enter port number:')
port = sys.stdin.readline().strip()

print('Enter method:')
method = sys.stdin.readline().strip()

if method == 'GET':
	r = requests.get('http://localhost:' + port + '/documents/test.txt')
elif method == 'POST':
	body = "Ice cream. Good morning!"
	while True:
		try:
			r = requests.post('http://localhost:' + port + '/documents/test.txt', data=body)
			break
		except:
			print("Connection refused. Trying again...")
			continue
elif method == 'PATCH':
	body = "Strawberry Ice cream!"
	while True:
		try:
			r = requests.patch('http://localhost:' + port + '/documents/test.txt', data=body)
			break
		except:
			print("Connection refused. Trying again...")
			continue
elif method == 'DELETE':
	r = requests.delete('http://localhost:' + port + '/documents/test.txt')
else:
	print('Invalid method')

print()
print("Status Code:", r.status_code)
print("-----")
print(r.text)
