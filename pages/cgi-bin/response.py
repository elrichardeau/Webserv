import os
import urllib.parse

# Get the query string from the environment
query = os.environ.get('QUERY_STRING', '')

# Manually parse the query string
def parse_query_string(query):
    params = {}
    if query:
        pairs = query.split('&')
        for pair in pairs:
            if '=' in pair:
                key, value = pair.split('=', 1)
                key = urllib.parse.unquote_plus(key)
                value = urllib.parse.unquote_plus(value)
                if key in params:
                    params[key].append(value)
                else:
                    params[key] = [value]
    return params

# Parse the query string
params = parse_query_string(query)

# Retrieve 'name' and 'age' values, default to 'N/A' if not found
name = params.get('name', ['N/A'])[0]
age = params.get('age', ['N/A'])[0]

print(f'''<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" type="text/css" href="../stylesResponse.css" />
    <title>Response</title>
</head>
<body>
    <h1 class="pagesTitle">Response</h1>
    <div class="response">
        <div class="tag">Name : <span class="value">{name}</span></div>
        <div class="tag">Age : <span class="value">{age}</span></div>
    </div>
    <div class="index">
        <a class="indexButton" href="/index.html">Index</a>
    </div>
</body>
</html>''')