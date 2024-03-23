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
			<input type="text" name="fname" placeholder="Enter Your Firstname" required><br>
			<input type="text" name="lname" placeholder="Enter  Your Latsname" required><br>
			<br>
			<button type="submit">Calculate</button>
		</form>
		<?php
	if (isset($_POST['fname']) && isset($_POST['lname']))
    { ?>
	<div class="calculator">
	
<?php
    
	echo "Your name is ".$_POST["fname"] . " " .$_POST["lname"] . "<br>";   
    }
?>
</div>
</div>
</body>
</html>