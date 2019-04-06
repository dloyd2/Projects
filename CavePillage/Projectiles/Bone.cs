using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bone : MonoBehaviour
{
    float timer = 4;
    int damage = 3;
    bool hit = false;
    [SerializeField] Animator anim;
    void Death()
    {
        anim.SetBool("dead", true);
        Destroy(gameObject, 0.5f);
    }

    // Update is called once per frame
    void Update()
    {
        timer -= Time.deltaTime;
        if(timer <= 0 || hit)
        {
            Death();
        }
    }
    
    void OnTriggerEnter2D(Collider2D other)
    {
        if(other.gameObject.layer == LayerMask.NameToLayer("Player") && !hit)
        {
            other.GetComponent<PlayerStats>().TakeDamage(damage);
            hit = true;
        }
      /*  else if(other.gameObject.layer == LayerMask.NameToLayer("Floor") && !hit)
        {
            hit = true;
        }*/
    }
    void OnCollisionEnter2D(Collision2D info)
    {
        hit = true;

    }
}
