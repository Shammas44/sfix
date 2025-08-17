import sys
import os

# Get the path to the project root (the directory containing src/ and tests/)
project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir))

# Add the 'src' directory to Python's sys.path
src_path = os.path.join(project_root, 'src')
if src_path not in sys.path:
    sys.path.insert(0, src_path) # Insert at the beginning to prioritize

# You can add a print statement to verify (remove after debugging)
# print(f"DEBUG: Added '{src_path}' to sys.path from tests/__init__.py")
# print(f"DEBUG: Current sys.path: {sys.path}")

# No other code needed for unittest __init__.py typically
