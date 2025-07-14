# Use a minimal official Python image for smaller footprint
FROM python:3.9-slim

# Set working directory inside the container
WORKDIR /app

# Copy and install Python dependencies
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy the application code
COPY app.py .

# Expose port 8080 and set default startup command
CMD ["python", "app.py"]