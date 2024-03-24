<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Upload Image</title>
    <style>
        .container {
            text-align: center;
        }
    </style>
</head>
<body>
<div class="container">
    <a href="./index.html">Home</a>
    <a href="./login.html">Login</a>
    <a href="/cgi-bin/" class="active">little Magic</a>
    <a href="./cookies">Cookies</a>
    <a href="./about-us.html">About us</a>
    <a href="./surprise.html">Surprise</a>
</div>
<div class="cgi">
    <div>
        <form action="postform.php" method="post" class="calculator" enctype="multipart/form-data">
            <input type="text" name="fname" placeholder="Enter Your Firstname" required><br>
            <input type="text" name="lname" placeholder="Enter Your Latsname" required><br>
            <input type="file" name="image" required><br> <!-- Add file input -->
            <br>
            <button type="submit">Submit</button>
        </form>
        <?php
        if (isset($_POST['fname']) && isset($_POST['lname']) && isset($_FILES['image'])) {
            $fname = $_POST['fname'];
            $lname = $_POST['lname'];
            $image = $_FILES['image'];

            // Check if image file is a valid image
            $check = getimagesize($image["tmp_name"]);
            if ($check !== false) {
                $targetDir = "./TESTIIII"; // Directory to store uploaded files
                $targetFile = $targetDir . basename($image["name"]);

                // Move the uploaded file to the target directory
                if (move_uploaded_file($image["tmp_name"], $targetFile)) {
                    echo "Your name is " . $fname . " " . $lname . "<br>";
                    echo "The file " . basename($image["name"]) . " has been uploaded.";
                } else {
                    echo "Sorry, there was an error uploading your file.";
                }
            } else {
                echo "File is not an image.";
            }
        }
        ?>
    </div>
</div>
</body>
</html>
