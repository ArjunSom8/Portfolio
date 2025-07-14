# Autonomous Vehicle Simulation

## Overview
This project presents an end-to-end autonomous vehicle simulation framework built on CARLA and Unreal Engine. It enables realistic vehicle dynamics, sensor integration, and controller tuning through digital twins, telemetry pipelines, and interactive dashboards. Designed for rapid prototyping and demonstration, it has been adopted by multiple AV courses.

## Features
- **Driveable Digital Twins**: Customized vehicle models in CARLA and Unreal Engine with LiDAR and camera integration.
- **Controller Tuning Dashboards**: Python/Pandas pipelines and Tableau-powered dashboards for real-time KPI visualization (speed, proximity, navigation accuracy).
- **Automated Telemetry Pipelines**: Data capture and processing scripts for streamlined analysis.
- **Demo Recordings & GitHub Codebase**: Well-documented multi-module repository with recorded drive-through scenarios.
- **Collaboration & Planning**: Facilitated over 20 technical and planning sessions, improving team coordination by 35%.

## Technologies
- **Languages & Frameworks**: Python, PyTorch, CARLA, Unreal Engine, Blender
- **Libraries & Tools**: OpenCV, YOLO, SSD, Pandas, Tableau, Excel
- **Repositories & Platforms**: GitHub, Multi-module CI/CD workflows

## Project Structure
```
avs/
├── car_models/             # Digital twin definitions and Blender assets
├── sensors/                # Camera & LiDAR integration scripts
├── telemetry/              # Data capture and processing pipelines
├── dashboards/             # Tableau workbooks & Python dashboard scripts
├── demos/                  # Recorded simulation videos
├── controllers/            # Vehicle control algorithms and tuning scripts
├── notebooks/              # Jupyter notebooks for analysis
└── README.txt              # This README file
```

## Installation
1. **Prerequisites**: Install [CARLA](https://carla.org/) (v0.9.x), Unreal Engine, Blender.
2. **Clone Repository**:
   ```bash
   git clone https://github.com/yourusername/avs.git
   cd avs
   ```
3. **Python Environment**:
   ```bash
   python3 -m venv venv
   source venv/bin/activate
   pip install -r requirements.txt
   ```
4. **Dashboard Setup**:
   - Open Tableau and load the workbooks in `dashboards/`.
   - Ensure data sources point to the output of the telemetry scripts.

## Usage
1. **Launch Simulation**:
   ```bash
   cd sensors
   python launch_sensors.py
   cd ../car_models
   python spawn_vehicle.py
   ```
2. **Run Controller & Telemetry**:
   ```bash
   cd ../controllers
   python run_controller.py --config ../configs/controller.yaml
   ```
3. **View Dashboards**:
   - Stream data into the dashboards for live KPI monitoring.
   - Use Jupyter notebooks in `notebooks/` for post-run analysis.

## Contributing
Contributions are welcome! Please follow these steps:
1. Fork the repository.
2. Create a feature branch (`git checkout -b feature/YourFeature`).
3. Commit your changes (`git commit -m 'Add YourFeature'`).
4. Push to the branch (`git push origin feature/YourFeature`).
5. Open a Pull Request.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
