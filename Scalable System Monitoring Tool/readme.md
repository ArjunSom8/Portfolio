# Scalable System Monitoring Tool

A production-grade monitoring and auto-healing platform for Kubernetes-based microservices architectures, demonstrating core Site Reliability Engineering (SRE) principles including observability, automation, and fault tolerance.

---

## Table of Contents

- [Features](#features)
- [Architecture](#architecture)
- [Prerequisites](#prerequisites)
- [Installation & Deployment](#installation--deployment)
- [Usage](#usage)
- [Directory Structure](#directory-structure)
- [Technologies](#technologies)
- [Contributing](#contributing)
- [License](#license)

---

## Features

- **Real-Time Observability**: Integrated Prometheus for metrics collection and Grafana for dashboarding.  
- **Auto-Healing**: Custom Kubernetes Operator for detecting anomalies and triggering pod restarts.  
- **Horizontal Scaling**: Kubernetes Horizontal Pod Autoscaler based on CPU utilization.  
- **Centralized Logging**: ELK Stack (Elasticsearch, Logstash, Kibana) for log aggregation and analysis.  
- **Infrastructure as Code**: Terraform scripts for GKE cluster provisioning.  
- **Declarative Deployments**: Helm chart for application management.  
- **CI/CD Pipeline**: GitHub Actions workflow automating build, push, and deployment to GKE.  
- **Chaos Testing**: Chaos Mesh experiments for fault injection and resilience validation.  

---

## Architecture



1. **Service Layer**: Python/Go microservice containerized with Docker.  
2. **Kubernetes Cluster**: GKE-hosted cluster orchestrating pods, services, and autoscaling.  
3. **Observability**: Prometheus scrapes metrics; Grafana provides visualization.  
4. **Logging**: Logstash ingests logs into Elasticsearch; Kibana for exploration.  
5. **Auto-Healing**: Kubernetes Operator watches Deployments and repairs failures.  
6. **Chaos Engineering**: Chaos Mesh injects pod-kill faults to test reliability.  

---

## Prerequisites

- Docker 19.03+  
- Kubernetes cluster (minikube or GKE)  
- kubectl CLI configured  
- Helm 3.x  
- Terraform 1.0+  
- GitHub Actions enabled on your repository  
- Go 1.16+ (for operator)  

---

## Installation & Deployment

1. **Clone the repository**:  
   ```bash
   git clone...
   cd scalable-system-monitoring-tool
   ```

2. **Provision infrastructure** (GKE cluster):  
   ```bash
   cd terraform
   terraform init
   terraform apply
   ```

3. **Deploy ELK stack**:  
   ```bash
   cd elk
   docker-compose up -d
   ```

4. **Deploy core services**:  
   ```bash
   kubectl apply -f k8s/namespace.yaml
   kubectl apply -f k8s/deployment.yaml
   kubectl apply -f k8s/service.yaml
   kubectl apply -f k8s/hpa.yaml
   ```

5. **Install Prometheus & Grafana**:  
   ```bash
   helm repo add prometheus-community https://prometheus-community.github.io/helm-charts
   helm install prometheus prometheus-community/prometheus
   helm repo add grafana https://grafana.github.io/helm-charts
   helm install grafana grafana/grafana
   ```

6. **Deploy custom operator**:  
   ```bash
   cd operator
   go build -o operator
   kubectl apply -f deploy/operator.yaml
   ```

7. **Run chaos experiments**:  
   ```bash
   kubectl apply -f chaos/pod-kill.yaml
   ```

---

## Usage

- Access Grafana: `kubectl port-forward svc/grafana 3000:80 -n sre-monitoring`  
- Access Kibana: `http://localhost:5601`  
- View Prometheus UI: `kubectl port-forward svc/prometheus-server 9090`  
- Check service health: `kubectl get pods -n sre-monitoring`  
- Observe auto-healing logs: `kubectl logs deployment/service-app -n sre-monitoring`  

---

## Directory Structure

```plaintext
├── service/            # Microservice application code + Dockerfile
├── k8s/                # Kubernetes manifests (Namespace, Deployment, Service, HPA)
├── prometheus/         # Prometheus configuration
├── grafana/            # Grafana dashboards
├── elk/                # ELK Stack setup
├── operator/           # Custom Kubernetes Operator (Go)
├── terraform/          # Terraform scripts for infrastructure
├── helm/               # Helm chart for service-app
├── .github/            # GitHub Actions CI/CD workflows
└── chaos/              # Chaos Mesh experiments
```

---

## Technologies

- **Containerization**: Docker  
- **Orchestration**: Kubernetes (GKE)  
- **Monitoring**: Prometheus, Grafana  
- **Logging**: ELK Stack  
- **Infrastructure as Code**: Terraform, Helm  
- **CI/CD**: GitHub Actions  
- **Chaos Engineering**: Chaos Mesh  
- **Programming**: Python, Go  

---

## Contributing

Contributions welcome! Please open issues or submit pull requests for bug fixes and enhancements.

1. Fork the repository  
2. Create a feature branch (`git checkout -b feature/YourFeature`)  
3. Commit your changes (`git commit -m 'Add some feature'`)  
4. Push to the branch (`git push origin feature/YourFeature`)  
5. Open a Pull Request  

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
