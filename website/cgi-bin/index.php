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
} else {
    // If JSON decoding failed, print an error message
    echo "Error: Unable to decode JSON data.";
}
?>
