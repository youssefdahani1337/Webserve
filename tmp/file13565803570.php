<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Calculator</title>
</head>
<body>
    <form action="calculate.php" method="post">
        <label for="num1">First number:</label>
        <input type="number" id="num1" name="num1" required><br>
        
        <label for="operator">Operator:</label>
        <select id="operator" name="operator" required>
            <option value="+">+</option>
            <option value="-">-</option>
            <option value="*">*</option>
            <option value="/">/</option>
        </select><br>
        
        <label for="num2">Second number:</label>
        <input type="number" id="num2" name="num2" required><br>
        
        <input type="submit" value="Calculate">
    </form>
</body>
</html>
