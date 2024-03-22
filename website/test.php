<?php

    // header("Location: http://www.youtube.com");
// Retrieve form data
$username = $_POST["username"];
$emailaddress = $_POST["emailaddress"];
// Print HTTP header
header("Content-type:text/html\r\n\r\n");

// Print HTML content
echo "<html>";
echo "<head>";
echo "<title>MY FIRST CGI FILE</title>";
echo "</head>";
echo "<body>";
echo "<h3>This is HTML's Body Section</h3>";
echo "<p>Username: $username</p>";
echo "<p>Email Address: $emailaddress</p>";
echo "</body>";
echo "</html>";

// phpinfo();
?>