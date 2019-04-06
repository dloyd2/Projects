using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Effect : MonoBehaviour
{
    GameObject attachedTo;  //who the effect is on
    
    public virtual void DoEffect()
    {
        Debug.Log("base effect called");
    }
    
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
