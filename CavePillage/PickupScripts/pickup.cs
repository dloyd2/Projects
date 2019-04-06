using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class pickup : MonoBehaviour
{
    protected bool pickedUp = false;
    protected float timer = 6f;
    public virtual void Death()
    {
        Destroy(gameObject);
        Debug.Log("Pick up is gone");
    }
    public virtual void PickedUpEffect(Collider2D other)
    {
        Debug.Log("parent PickedUp called");
    }
    public virtual void UpdateChanges()
    {
        
    }
    // Start is called before the first frame update
    void Start()
    {
        
    }
    public void Update()
    {
        timer -= Time.deltaTime;
        if(timer <= 0f)
        {
            Death();
        }
        if(pickedUp)
        {
            Death();
        }
        UpdateChanges();
    }
    void OnTriggerEnter2D(Collider2D other)
    {
        if(other.gameObject.layer == LayerMask.NameToLayer("Player") && !pickedUp)
        {
            pickedUp = true;
            PickedUpEffect(other);
        }
    }
}
