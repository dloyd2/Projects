using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HealthPickup : pickup
{
    int healthBack = 2;
   
    public override void PickedUpEffect(Collider2D other)
    {
        other.gameObject.GetComponent<PlayerStats>().HealHealth(healthBack);
    }
}
