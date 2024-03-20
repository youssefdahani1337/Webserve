<!DOCTYPE html>
<html lang="en">
	<head>
		<title>my Simple Website</title>
		<meta charset="UTF-8">
		<link href="../style.css" rel="stylesheet">
		<link rel="preconnect" href="https://fonts.googleapis.com">
		<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
		<link href="https://fonts.googleapis.com/css2?family=Madimi+One&display=swap" rel="stylesheet">
	</head>
	<body>
		<div class="container">
			<a href="../index.html" >Home</a>
			<a href="../login.html">Login</a>
			<a href="./index.html" class="active">little Magic</a>
			<a href="../cookies/">Cookies</a>
			<a href="../about-us.html">About us</a>
			<a href="../surprise.html">Surprise</a>
		</div>
<?php
session_start();

// Check if form is submitted via GET method
if ($_SERVER["REQUEST_METHOD"] == "GET") {
    // Mocking user credentials (replace with your own authentication logic)
    $valid_username = "admin";
    $valid_password = "password";

    // Get username and password from query string
    $username = $_GET["username"];
    $password = $_GET["password"];

    // Check if credentials are valid
    if ($username === $valid_username && $password === $valid_password) {
        // Set session variables
        $_SESSION["username"] = $username;

        // Redirect to dashboard or any other authenticated page
        echo "<h2>Logged</h2>";
        exit();
    } else {
        echo "<h2> Incorrect username or password</h2>";
        exit();
    }
} else {
    // If user accesses login.php directly, redirect to login page
    header("Location: http://127.0.0.1");
    exit();
}
?>
</body>
</html>