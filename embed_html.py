# -*- coding: utf-8 -*-
Import("env")
import os
print(">>> Running embed_html.py <<<")
def embed_html(source, target, env):
    html_path = os.path.join("data", "webApp.html")
    cpp_path = os.path.join("src",'apps', "webApp.h")

    if not os.path.exists(html_path):
        print("⚠️ File not found:", html_path)
        return

    with open(html_path, "r", encoding="utf-8") as f:
        content = f.read()

    # No need to escape double quotes when using rawliteral
    # But replace triple quotes to avoid syntax issues
    content = content.replace('"""', '\\"""')

    # Generate the header file
    with open(cpp_path, "w", encoding="utf-8") as f:
        f.write('// ⚠️ This file is auto-generated - DO NOT EDIT MANUALLY\n')
        f.write('const char html[] PROGMEM = R"rawliteral(' + content + ')rawliteral";\n')

    print(f"✅ HTML embedded -> {cpp_path}")

embed_html(None, None, None)
# Automatically run before the build process
env.AddPreAction("buildprog", embed_html)
