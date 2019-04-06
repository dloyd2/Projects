using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class thowingKnife : MonoBehaviour
{
    bool hit = false;
    float despawnTimer = 3f;
    int damage = 5;
    
    void Death()
    {
        Destroy(gameObject);
    }

    // Update is called once per frame
    void Update()
    {
        despawnTimer -= Time.deltaTime;
        if(despawnTimer <= 0 || hit)
        {
            Death();
        }
    }
    void OnCollisionEnter2D(Collision2D info)
    {
        if(info.gameObject.layer == LayerMask.NameToLayer("Player") && !hit)
        {
                info.gameObject.GetComponent<PlayerStats>().TakeDamage(damage);
        }
        hit = true;
    }
}
