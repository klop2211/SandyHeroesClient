using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BoxColliderAutoResizer : MonoBehaviour
{
	[ContextMenu("Box Collider �ڵ� ����")]
	void ResizeBoxColliderToFitChildren()
	{
		BoxCollider collider = GetComponent<BoxCollider>();
		if (collider == null)
		{
			collider = gameObject.AddComponent<BoxCollider>();
		}

		Bounds bounds = new Bounds(transform.position, Vector3.zero);
		bool hasBounds = false;

		Renderer[] renderers = GetComponentsInChildren<Renderer>();

		foreach (Renderer rend in renderers)
		{
			if (rend == null || rend.gameObject == this.gameObject) continue;

			if (!hasBounds)
			{
				bounds = rend.bounds;
				hasBounds = true;
			}
			else
			{
				bounds.Encapsulate(rend.bounds);
			}
		}

		if (hasBounds)
		{
			Vector3 localCenter = transform.InverseTransformPoint(bounds.center);
			Vector3 localSize = transform.InverseTransformVector(bounds.size);

			collider.center = localCenter;
			collider.size = localSize;
		}
		else
		{
			Debug.LogWarning("�ڽ� �������� ���ų� �ٿ�带 ����� �� �����ϴ�.");
		}
	}
}