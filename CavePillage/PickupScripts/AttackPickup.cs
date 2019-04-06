using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AttackPickup : pickup
{
    int attackIncrease = 1;
    Color startColor = new Color(1, 1, 1, 1);
    Color endColor   = new Color(1, 1, 1, 0.15f);
    Color tempColor;
    [SerializeField]SpriteRenderer sprite;
    public override void UpdateChanges()
    {
        tempColor = Color.Lerp(startColor, endColor, (6-timer)/6);
        sprite.color = tempColor;
    }
    public override void PickedUpEffect(Collider2D other)
    {
        other.gameObject.GetComponent<PlayerAttack>().ChangeMeleeDamage(attackIncrease);
    }
}
