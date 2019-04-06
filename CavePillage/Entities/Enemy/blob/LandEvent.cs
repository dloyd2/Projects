using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LandEvent : MonoBehaviour
{
    [SerializeField] BlobMovement move;
    void Awaken()
    {
        this.enabled = false;
    }
    // Update is called once per frame
    void Update()
    {
        gameObject.SendMessage("UpdateMove");
        this.enabled = false;
    }
}
