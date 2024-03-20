<?php

// Read the JSON data from CGI input
$json_data = file_get_contents('php://input');

// Check if JSON data is received
if ($json_data === false) {
    http_response_code(400); // Bad Request
    echo "Error: No JSON data received";
    exit;
}

// Decode the JSON data
$decoded_data = json_decode($json_data, true);

// Check if JSON decoding was successful
if ($decoded_data === null) {
    http_response_code(400); // Bad Request
    echo "Error: Invalid JSON data";
    exit;
}

// Process the JSON data (for example, just echoing it back)
echo "Received JSON data:\n";
print_r($decoded_data);

// You can perform any other operations with the decoded JSON data here

?>
