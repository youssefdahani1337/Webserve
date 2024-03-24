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
			<a href="../session/">Login</a>
			<a href="./index.html" class="active">little Magic</a>
			<a href="../cookies/">Cookies</a>
			<a href="../about-us.html">About us</a>
			<a href="../surprise.html">Surprise</a>
		</div>
        <div class="cgi">
	    <div>
		<form action="postform.php" method="post" class="calculator">
			<input type="text" name="fname" placeholder="Enter Your Firstname" required><br>
			<input type="text" name="lname" placeholder="Enter  Your Lastname" required><br>
			<br>
			<button type="submit">Calculate</button>
		</form>

<?php
// Get the JSON data from CGI input
$json_data = file_get_contents('php://input');

// Decode the JSON data
$data = json_decode($json_data, true);

// Check if JSON decoding was successful
if ($data !== null) {
    // Start HTML output
    echo "<!DOCTYPE html>\n";
    echo "<html>\n";
    echo "<head>\n";
    echo "<title>JSON Content</title>\n";
    echo "</head>\n";
    echo "<body>\n";
    echo "<h1>JSON Content:</h1>\n";
    
    // Output each key-value pair from JSON data
    echo "<ul>\n";
    foreach ($data as $key => $value) {
        echo "<li><strong>$key:</strong> $value</li>\n";
    }
    echo "</ul>\n";

    // End HTML output
    echo "</body>\n";
    echo "</html>\n";

}
?>
</div>
</body>
</html>