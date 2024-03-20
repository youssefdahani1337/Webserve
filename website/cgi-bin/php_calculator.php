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
		<form action="php_calculator.php" method="get" class="calculator">
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

<?php
	if (isset($_GET['num1']) && isset($_GET['num2']) && isset($_GET['operator'])) {
?>
<div class="calculator">

	<?php
	$num1 = floatval($_GET['num1']);
	$num2 = floatval($_GET['num2']);
	$operator = $_GET['operator'];
	
	switch ($operator) {
		case 'add':
		$result = $num1 + $num2;
		break;
		case 'subtract':
		$result = $num1 - $num2;
		break;
		case 'multiply':
		$result = $num1 * $num2;
		break;
		case 'divide':
		if ($num2 != 0) {
			$result = $num1 / $num2;
		} else {
			$result = "Cannot divide by zero";
		}
		break;
		default:
		$result = "Invalid operation";
		break;
	}
	echo '<div class="result">Result: ' . $result . '</div></div>';
}
?>
	</div>	</div>
</body>
</html>
