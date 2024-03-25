#!/usr/bin/env python

print("Content-type: text/html\r\n\r\n")  # Required header for CGI scripts

html_content = """
<html lang="en">
	<head>
		<title>my Simple Website</title>
		<meta charset="UTF-8">
		<link href="../style.css" rel="stylesheet">
		<link rel="preconnect" href="https://fonts.googleapis.com">
		<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
		<link href="https://fonts.googleapis.com/css2?family=Madimi+One&display=swap" rel="stylesheet">
		<style>
		</style>
	</head>
	<body>
    <div class="container">
			<a href="../index.html" >Home</a>
			<a href="../session/">Login</a>
			<a href="./index.html" class="active">little Magic</a>
			<a href="../cookies/">Cookies</a>
			<a href="../about-us.html">About us</a>
			<a href="../surprise.html">Surprise</a>
		</div>
    <div class="cgi">
	<div >
		<form class="calculator" action="py_calculator.py" method="get">
			<input type="number" name="num1" placeholder="Enter first number" required><br>
			<input type="number" name="num2" placeholder="Enter second number" required><br>
			<select name="operator">
				<option value="add">Addition (+)</option>
				<option value="subtract">Subtraction (-)</option>
				<option value="multiply">Multiplication (*)</option>
				<option value="divide">Division (/)</option>
			</select><br>
			<button type="submit">Calculate</button>
		</form>
    %s
</body>
</html>
"""

import cgi

form = cgi.FieldStorage()

if form.getvalue('num1') and form.getvalue('num2') and form.getvalue('operator'):
	print("HElllo")
    num1 = float(form.getvalue('num1'))
    num2 = float(form.getvalue('num2'))
    operation = form.getvalue('operator')

    if operation == 'add':
        result = num1 + num2
    elif operation == 'subtract':
        result = num1 - num2
    elif operation == 'multiply':
        result = num1 * num2
    elif operation == 'divide':
        if num2 == 0:
            result = "Error: Cannot divide by zero!"
        else:
            result = num1 / num2
    else:
        result = "Invalid operation"
    print(html_content % ("<div class='calculator'><div class='result'>Result: %s </div>" % result,))  
    print("</div>")
else:
    print(html_content % "\n")