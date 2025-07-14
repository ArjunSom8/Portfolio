# Configure the GCP provider
provider "google" {
  project = var.project
  region  = var.region
}

# Create a GKE cluster for hosting our services
resource "google_container_cluster" "primary" {
  name               = "sre-monitoring-cluster"
  location           = var.region
  initial_node_count = 3        # Start with 3 nodes for redundancy

  node_config {
    machine_type = "e2-medium"  # Balance cost and performance
  }
}