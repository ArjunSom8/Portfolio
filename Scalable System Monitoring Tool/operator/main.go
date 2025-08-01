package main

import (
	"context"
	"log"
	"time"

	appsv1 "k8s.io/api/apps/v1"
	corev1 "k8s.io/api/core/v1"
	"k8s.io/apimachinery/pkg/runtime"
	"k8s.io/apimachinery/pkg/types"
	ctrl "sigs.k8s.io/controller-runtime"
	"sigs.k8s.io/controller-runtime/pkg/client"
)

// DeploymentReconciler watches Deployments and performs auto-healing logic
type DeploymentReconciler struct {
	client.Client
	Scheme *runtime.Scheme
}

func (r *DeploymentReconciler) Reconcile(ctx context.Context, req ctrl.Request) (ctrl.Result, error) {
	log.Printf("Reconciling Deployment: %s", req.Name)

	// Fetch the Deployment instance
	var deployment appsv1.Deployment
	if err := r.Get(ctx, req.NamespacedName, &deployment); err != nil {
		log.Printf("Failed to get Deployment: %v", err)
		return ctrl.Result{}, client.IgnoreNotFound(err)
	}

	// Simple auto-healing logic: if ready replicas are less than desired, restart pods
	if *deployment.Spec.Replicas > 0 && deployment.Status.ReadyReplicas < *deployment.Spec.Replicas {
		log.Printf("Deployment %s is unhealthy. Ready replicas: %d, Desired replicas: %d",
			req.Name, deployment.Status.ReadyReplicas, *deployment.Spec.Replicas)

		// Restart pods by deleting them
		if err := r.restartPods(ctx, &deployment); err != nil {
			log.Printf("Failed to restart pods for Deployment %s: %v", req.Name, err)
			return ctrl.Result{}, err
		}
		log.Printf("Successfully restarted pods for Deployment %s", req.Name)
	}

	return ctrl.Result{RequeueAfter: time.Minute}, nil
}

func (r *DeploymentReconciler) restartPods(ctx context.Context, deployment *appsv1.Deployment) error {
	podList := &corev1.PodList{}
	listOpts := []client.ListOption{
		client.InNamespace(deployment.Namespace),
		client.MatchingLabels(deployment.Spec.Selector.MatchLabels),
	}
	if err := r.List(ctx, podList, listOpts...); err != nil {
		return err
	}

	for _, pod := range podList.Items {
		log.Printf("Deleting pod: %s", pod.Name)
		if err := r.Delete(ctx, &pod); err != nil {
			return err
		}
	}

	return nil
}

func (r *DeploymentReconciler) SetupWithManager(mgr ctrl.Manager) error {
	return ctrl.NewControllerManagedBy(mgr).
		For(&appsv1.Deployment{}). // Watch Deployment resources
		Complete(r)
}

func main() {
	scheme := runtime.NewScheme()
	_ = appsv1.AddToScheme(scheme)
	_ = corev1.AddToScheme(scheme)

	mgr, err := ctrl.NewManager(ctrl.GetConfigOrDie(), ctrl.Options{Scheme: scheme})
	if err != nil {
		log.Fatal(err)
	}

	// Register our reconciler with the manager
	if err := (&DeploymentReconciler{
		Client: mgr.GetClient(),
		Scheme: mgr.GetScheme(),
	}).SetupWithManager(mgr); err != nil {
		log.Fatal(err)
	}

	// Start the controller loop
	log.Println("Starting manager")
	if err := mgr.Start(ctrl.SetupSignalHandler()); err != nil {
		log.Fatal(err)
	}
}
