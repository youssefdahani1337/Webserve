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
			<a href="../session/index.html">Login</a>
			<a href="../cgi-bin/">little Magic</a>
			<a href="./index.html" class="active">Cookies</a>
			<a href="../about-us.html">About us</a>
			<a href="../surprise.html">Surprise</a>
		</div>


            <div class="cookies">
        <br><br>
        <?php
    if (count($_COOKIE) > 0) {
        echo "<h2>All Cookies</h2>";
        echo "<table>";
        echo "<tr><th>Name</th><th>Value</th></tr>";

        foreach ($_COOKIE as $name => $value) {
            echo "<tr><td>$name</td><td>$value</td></tr>";
        }

        echo "</table>";
    }
     else {
        echo "No cookies set.";
    }
    ?>
            <?php
        if(isset($_GET['key']) && isset($_GET['value'])) {
            $key = $_GET['key'];
            $value = $_GET['value'];
            setcookie($key, $value, time() + (86400 * 30), "/");
            echo "<br><p>Cookie '$key' is set with value '$value'! </p><br>";
        } else {
            echo "<p>Please provide both key and value!</p>";
        }
        ?>
    </div>
</body>
</html>
<!DOCTYPE html>
<html>
<head>
    <title>Cookie Table</title>
    <style>

    </style>
</head>
<body>