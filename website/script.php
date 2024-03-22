<?php

$endpointURL = 'http://localhost:8001/cgi-bin/script.php'; 
$data = array('key' => 'value'); 

function sendPostRequest($url, $data) {
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_POST, false);
    curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($data));
    curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type: video/mp4'));

    $response = curl_exec($ch);
    $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);

    if ($httpCode === 200) {
        echo 'Request sent successfully' . PHP_EOL . "<br>";
    } else {
        echo 'Request failed with HTTP code ' . $httpCode . PHP_EOL ."<br>";
    }

    curl_close($ch);
}

function sendMultipleRequests($url, $data, $count) {
    for ($i = 0; $i < $count; $i++) {
        sendPostRequest($url, $data);
    }

    echo "done";
}

sendMultipleRequests($endpointURL, $data, 10);

?>