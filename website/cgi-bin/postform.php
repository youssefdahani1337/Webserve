<!DOCTYPE html>
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
			<a href="./index.html" >Home</a>
			<a href="./login.html">Login</a>
			<a href="/cgi-bin/" class="active">little Magic</a>
			<a href="./cookies">Cookies</a>
			<a href="./about-us.html">About us</a>
			<a href="./surprise.html">Surprise</a>
	</div>
    <div class="cgi">
	<div >
		<form action="postform.php" method="post" class="calculator">
			<input type="number" name="num1" pla0ceholder="Enter first number" required><br>
			<input type="number" name="num2" placeholder="Enter second number" required><br>
			<select name="operator">
				<option value="add">Addition (+)</option>
				<option value="subtract">Subtraction (-)</option>
				<option value="multiply">Multiplication (*)</option>
				<option value="divide">Division (/)</option>
			</select><br>
			<button type="submit">Calculate</button>
		</form>
		<?php
	if (isset($_POST['num1']) && isset($_POST['num2']))
    {
        echo $POST["num1"] . " " .$_POST["num2"] . "<br>";   
    }
?>

</body>
</html>